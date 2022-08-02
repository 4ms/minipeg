#include "stm32xx.h"

#define USING_FSK
#ifndef USING_FSK
#define USING_QPSK
#endif

#include "../system.hh"
#include "bootloader/animation.hh"
#include "bootloader/buttons.hh"
#include "bootloader/gate_input.hh"
#include "bootloader/leds.hh"
#include "dig_inouts.hh"
#include "drivers/system.hh"
#include "flash.hh"
#include "flash_layout.hh"

#include "bl_utils.hh"

#ifdef USING_QPSK
#include "stm_audio_bootloader/qpsk/demodulator.h"
#include "stm_audio_bootloader/qpsk/packet_decoder.h"
#else
#include "stm_audio_bootloader/fsk/demodulator.h"
#include "stm_audio_bootloader/fsk/packet_decoder.h"
#endif

using namespace stmlib;
using namespace stm_audio_bootloader;

#ifdef USING_QPSK
const float kModulationRate = 6000.0;
const float kBitRate = 12000.0;
const float kSampleRate = 48000.0;
#else
const uint32_t kSampleRate = 22050;
#endif
uint32_t kStartExecutionAddress = AppFlashAddr;
uint32_t kStartReceiveAddress = BootloaderReceiveAddr;

const uint32_t kBlkSize = 16384;
const uint16_t kPacketsPerBlock = kBlkSize / kPacketSize; //kPacketSize=256
uint8_t recv_buffer[kBlkSize];

volatile uint32_t systmr = 0;
PacketDecoder decoder;
Demodulator demodulator;

uint16_t packet_index;
uint16_t discard_samples = 8000;
uint32_t current_address;

enum UiState { UI_STATE_WAITING, UI_STATE_RECEIVING, UI_STATE_ERROR, UI_STATE_WRITING, UI_STATE_DONE };
volatile UiState ui_state;

//FIXME: use this
void read_gate_input() {
	// DEBUG1_ON;
	bool sample = gate_in_read();
	// if (sample) DEBUG1_ON;
	// else DEBUG1_OFF;
	if (!discard_samples) {
		demodulator.PushSample(sample ? 1 : 0);
	} else {
		--discard_samples;
	}
	// DEBUG1_OFF;
}

static void animate_until_button_pushed(Animations animation_type, Button button);
static void update_LEDs();
static void init_fsk();
static void delay(uint32_t tm);

void main() {
	uint32_t symbols_processed = 0;
	uint32_t dly = 0, button_debounce = 0;
	uint8_t do_bootloader;
	uint8_t symbol;
	PacketDecoderState state;
	bool rcv_err;
	uint8_t exit_updater = false;

	mdrivlib::System::SetVectorTable(BootloaderFlashAddr);
	system_init();

	delay(300);

	init_leds();
	init_buttons();
	init_gate_in();

	animate(ANI_RESET);

	dly = 32000;
	while (dly--) {
		if (button_pushed(Button::Ping) && button_pushed(Button::Cycle))
			button_debounce++;
		else
			button_debounce = 0;
	}
	do_bootloader = (button_debounce > 15000) ? 1 : 0;

	delay(100);

	DigIO::ClockBusOut init;
	DigIO::ClockBusOut::high();
	DigIO::ClockBusOut::low();
	DigIO::EOJack::high();
	DigIO::EOJack::low();

	if (do_bootloader) {
		while (button_pushed(Button::Ping) || button_pushed(Button::Cycle))
			;


		start_reception(kSampleRate, [&]() {
			DigIO::ClockBusOut::high();
			bool sample = gate_in_read();
			if (!discard_samples) {
				demodulator.PushSample(sample ? 1 : 0);
			} else {
				--discard_samples;
			}
			DigIO::ClockBusOut::low();
		});

		delay(100);

		uint32_t button1_exit_armed = 0;
		uint32_t button2_exit_armed = 0;

		while (!exit_updater) {
			rcv_err = false;

			while (demodulator.available() && !rcv_err && !exit_updater) {
				symbol = demodulator.NextSymbol();
				state = decoder.ProcessSymbol(symbol);
				symbols_processed++;

				switch (state) {
					case PACKET_DECODER_STATE_OK:
						ui_state = UI_STATE_RECEIVING;
						memcpy(recv_buffer + (packet_index % kPacketsPerBlock) * kPacketSize,
							   decoder.packet_data(),
							   kPacketSize);
						++packet_index;
						if ((packet_index % kPacketsPerBlock) == 0) {
							ui_state = UI_STATE_WRITING;

							//Check for valid flash address before writing to flash
							if ((current_address + kBlkSize) <= get_sector_addr(NumFlashSectors)) {
								flash_write_page(recv_buffer, current_address, kBlkSize);
								current_address += kBlkSize;
							} else {
								ui_state = UI_STATE_ERROR;
								rcv_err = true;
							}
							decoder.Reset();

#ifndef USING_QPSK
							demodulator.Sync(); //FSK
#else
							demodulator.SyncCarrier(false); //QPSK
#endif

						} else {
#ifndef USING_QPSK
							decoder.Reset(); //FSK
#else
							demodulator.SyncDecision();		//QPSK
#endif
						}
						break;

					case PACKET_DECODER_STATE_ERROR_SYNC:
						rcv_err = true;
						break;

					case PACKET_DECODER_STATE_ERROR_CRC:
						rcv_err = true;
						break;

					case PACKET_DECODER_STATE_END_OF_TRANSMISSION:
						exit_updater = true;
						ui_state = UI_STATE_DONE;
						animate_until_button_pushed(ANI_SUCCESS, Button::Ping);
						animate(ANI_RESET);
						delay(100);
						break;

					default:
						break;
				}
			}
			if (rcv_err) {
				ui_state = UI_STATE_ERROR;
				animate_until_button_pushed(ANI_FAIL_ERR, Button::Ping);
				animate(ANI_RESET);
				delay(100);
				init_fsk();

				exit_updater = false;
			}

			if (button_pushed(Button::Cycle)) {
				if (button2_exit_armed) {
					if (packet_index == 0)
						exit_updater = true;
				}
				button2_exit_armed = 0;
			} else
				button2_exit_armed = 1;

			if (button_pushed(Button::Ping)) {
				if (button1_exit_armed) {
					if (ui_state == UI_STATE_WAITING)
						exit_updater = true;
				}
				button1_exit_armed = 0;
			} else
				button1_exit_armed = 1;
		}
		ui_state = UI_STATE_DONE;
		while (button_pushed(Button::Ping) || button_pushed(Button::Cycle)) {
			;
		}
	}

	reset_buses();
	reset_RCC();
	jump_to(kStartExecutionAddress);
}

void init_fsk() {
#ifdef USING_QPSK
	//QPSK
	decoder.Init((uint16_t)20000);
	demodulator.Init(
		kModulationRate / kSampleRate * 4294967296.0, kSampleRate / kModulationRate, 2.0 * kSampleRate / kBitRate);
	demodulator.SyncCarrier(true);
	decoder.Reset();
#else
	//FSK
	decoder.Init();
	decoder.Reset();
	demodulator.Init(16, 8, 4); //pause, one, zero. pause_thresh = 24. one_thresh = 6.
	demodulator.Sync();
#endif

	current_address = kStartReceiveAddress;
	packet_index = 0;
	ui_state = UI_STATE_WAITING;
}

void update_LEDs() {
	if (ui_state == UI_STATE_RECEIVING)
		animate(ANI_RECEIVING);

	else if (ui_state == UI_STATE_WRITING)
		animate(ANI_WRITING);

	else if (ui_state == UI_STATE_WAITING)
		animate(ANI_WAITING);

	else //if (ui_state == UI_STATE_DONE)
	{}
}

void animate_until_button_pushed(Animations animation_type, Button button) {
	animate(ANI_RESET);

	while (!button_pushed(button)) {
		delay(1);
		animate(animation_type);
	}
	while (button_pushed(button)) {
		delay(1);
	}
}

void delay(uint32_t ticks) {
	uint32_t i = systmr;
	while ((systmr - i) < ticks) {
		;
	}
}

extern "C" void SysTick_Handler(void) {
	systmr = systmr + 1;
	update_LEDs();
}

extern "C" void NMI_Handler() {
	__BKPT();
}
extern "C" void HardFault_Handler() {
	__BKPT();
}
extern "C" void MemManage_Handler() {
	__BKPT();
}
extern "C" void BusFault_Handler() {
	__BKPT();
}
extern "C" void UsageFault_Handler() {
	__BKPT();
}
extern "C" void SVC_Handler() {
	__BKPT();
}
extern "C" void DebugMon_Handler() {
	__BKPT();
}
extern "C" void PendSV_Handler() {
	__BKPT();
}

// } //extern "C"
