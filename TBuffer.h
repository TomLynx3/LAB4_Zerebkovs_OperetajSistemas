#pragma once
#define BUF_SIZE 100
#include <windows.h>


struct TBufItem { int tu; int tk; };
class TBuffer {
private:
	TBufItem buf[BUF_SIZE]; 
	int head;
	CRITICAL_SECTION cs;
	int midProc;
	int maxProc;
	//CRITICAL_SECTION cs; // kritiska sekcija
	//...

public:
	TBuffer();
	~TBuffer();
	void Draw(HDC hdc, RECT rect);
	void Put(TBufItem AItem);
	void Clear();
	int GetMidProc();
	int GetMaxProc();
	void IncreaseProcessPercentage();
	void DecreaseProcessPercentage();
	//...
};