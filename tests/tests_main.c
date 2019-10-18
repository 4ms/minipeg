#include <stdio.h>
/*Unity Framework*/
#include<setjmp.h>
#include "unity.h"
/*Test functions*/
#include "test_flash_user.h"

void runTest(UnityTestFunction test)
{
	if(TEST_PROTECT())
	{
		test();
	}
}

int main()
{
	
	printf("\n\nTests: \n");
	
	/**/
	UnityBegin("test_flash_user.c");
	RUN_TEST(testFlashWrites);
	
	return (UnityEnd());

}
