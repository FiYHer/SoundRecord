
#include "SoundRecord.h"
#include <stdio.h>

int main()
{
	SoundRecord MyTest;
	while (true)
	{
		DWORD dwSize = 0;
		LPBYTE pBuffer = MyTest.GetSoundBuffer(&dwSize);
		if(pBuffer!=nullptr)
		{
			printf("��ȡ�������ݳɹ�\n");
			if (MyTest.PlaySoundBuffer(pBuffer, dwSize)==TRUE)
			{
				printf("���������ɹ�\n");
			}
		}
	}
	return 0;
}
