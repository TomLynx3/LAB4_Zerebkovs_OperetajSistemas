#include "TBuffer.h"
#include <synchapi.h>


TBuffer::TBuffer()
{
	this->head = 0;

	this->buf[BUF_SIZE] = {};

	this->midProc = 25;

	this->maxProc = 50;

	InitializeCriticalSection(&this->cs);
	
}

TBuffer::~TBuffer()
{
	DeleteCriticalSection(&this->cs);
}

void TBuffer::Draw(HDC dc, RECT rect)
{


	HBRUSH brush = CreateSolidBrush(RGB(50, 151, 151));
	HPEN user_pen = CreatePen(PS_SOLID, 2, RGB(89, 41, 71));
	HPEN kernel_pen = CreatePen(PS_SOLID, 2, RGB(46, 244, 61));
	Rectangle(dc, rect.left, rect.top, rect.right, rect.bottom);

	FillRect(dc, &rect, brush);

	
	if (this->head != 0) {
		EnterCriticalSection(&this->cs);
		int index = this->head;
		int t = 0;


		int xPrev = 0;
		int yUserPrev = 0;
		int yKernelPrev = 0;

		while (index != this->head - 1) {

			TBufItem item = this->buf[index];

			int x = t * rect.right / (BUF_SIZE - 1);

			int y = rect.bottom - ((rect.bottom * item.tu) / this->maxProc);

			int ykernel = rect.bottom - ((rect.bottom * item.tk) / this->maxProc);

			

			if (t==0) {

				SelectObject(dc, user_pen);
				MoveToEx(dc, x,y, NULL);

				SelectObject(dc, kernel_pen);
				MoveToEx(dc, x, ykernel, NULL);

				yUserPrev = y;
				yKernelPrev = ykernel;
				xPrev = x;
			}
			else {
				SelectObject(dc, user_pen);
				MoveToEx(dc, xPrev, yUserPrev, NULL);
				LineTo(dc, x, y);


				SelectObject(dc, kernel_pen);
				MoveToEx(dc, xPrev, yKernelPrev, NULL);
				LineTo(dc, x, ykernel);

				yUserPrev = y;
				yKernelPrev = ykernel;
				xPrev = x;

			}
			index = (index + 1) % BUF_SIZE;
			t++;

		}
		

		LeaveCriticalSection(&this->cs);

		DeleteObject(brush);
		DeleteObject(user_pen);
		DeleteObject(kernel_pen);
	}
}


void TBuffer::Put(TBufItem AItem) {

	EnterCriticalSection(&this->cs);

	this->buf[this->head] = AItem;

	this->head = (this->head + 1) % BUF_SIZE;

	LeaveCriticalSection(&this->cs);
}

void TBuffer::Clear()
{
	memset(this->buf, 0, sizeof(this->buf));
}

int TBuffer::GetMidProc() {
	return this->midProc;
}

int TBuffer::GetMaxProc() {
	return this->maxProc;
}

void  TBuffer::IncreaseProcessPercentage() {
	this->maxProc = this->maxProc + 10;

	this->midProc = this->maxProc / 2;
}

void TBuffer::DecreaseProcessPercentage() {
	this->maxProc = this->maxProc - 10;

	this->midProc = this->maxProc / 2;
}
