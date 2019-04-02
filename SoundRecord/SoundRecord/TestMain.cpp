
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
			printf("获取声音数据成功\n");
			if (MyTest.PlaySoundBuffer(pBuffer, dwSize)==TRUE)
			{
				printf("播放声音成功\n");
			}
		}
	}
	return 0;
}
