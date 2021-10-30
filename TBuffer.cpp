#include "TBuffer.h"
#include <synchapi.h>


TBuffer::TBuffer()
{
	this->head = 0;

	this->buf[BUF_SIZE] = {};

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

		while (index != this->head - 1) {
			TBufItem item = this->buf[index];


			int x = (rect.right / BUF_SIZE - 1) * t;
			int y = rect.bottom - ((rect.bottom * item.tu) / 100);

			int ykernel = rect.bottom - ((rect.bottom * item.tk) / 100);

			index = (index + 1) % BUF_SIZE;

			if (t==0) {
				MoveToEx(dc, x,rect.bottom, NULL);
			}
			else {
				SelectObject(dc, user_pen);
				LineTo(dc, x, y);
				SelectObject(dc, kernel_pen);
				LineTo(dc, x, ykernel);

			}

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



