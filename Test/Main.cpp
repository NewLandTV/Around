#include<stdio.h>
#include<clocale>
#include<string>
#include<Windows.h>
int x;std::wstring u(L"");inline void E(std::wstring*c){for(size_t f=0;f<c->size();f=f+1)c->at(f)^=-13;std::reverse(c->begin(),c->end());}
long long __stdcall kill(HWND a,UINT b,WPARAM c,LPARAM d){RECT qw;HDC q,r,x;PAINTSTRUCT cu;HBITMAP la,c9w3;
switch(b){case 0x000f:GetClientRect(a,&qw);q=BeginPaint(a,&cu);r=CreateCompatibleDC(q);la=CreateCompatibleBitmap(q,qw.right,qw.bottom);
c9w3=reinterpret_cast<HBITMAP>(SelectObject(r,la));PatBlt(r,0,NULL,qw.right,qw.bottom,BLACKNESS);x=q;q=r;r=x;
TextOutW(q,NULL,NULL,u.c_str(),u.size());
x=q;q=r;r=x;GetClientRect(a,&qw);
BitBlt(q,NULL,NULL,qw.right,qw.bottom,r,0,0,static_cast<unsigned long>(0x00CC0020));SelectObject(r,c9w3);DeleteObject(c9w3);DeleteDC(r);EndPaint(a,&cu);
break;case WM_DESTROY:PostQuitMessage(0);break;default:return DefWindowProc(a,b,c,d);}return 0;}
int __stdcall WinMain(_In_ HINSTANCE o,_In_opt_ HINSTANCE v,_In_ LPSTR n_,_In_ int ei){HWND z;MSG orr;WNDCLASSEX out;setlocale(NULL,"");
_iobuf*d;if(fopen_s(&d,"C:\\NewLand\\Around\\Data.adat","r,ccs=UTF-8"))return -1;wchar_t r[2048];x=fread(r,sizeof(wchar_t),1023,d);fclose(d);if(!x)return -1;u=std::wstring(r);E(&u);
out.cbClsExtra=NULL;out.cbSize=sizeof(WNDCLASSEX);out.cbWndExtra=NULL;out.hbrBackground=reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
out.hCursor=LoadCursor(nullptr,MAKEINTRESOURCEW(32512));out.hIcon=LoadIcon(o,MAKEINTRESOURCEW(32512));out.hIconSm=LoadIcon(o,MAKEINTRESOURCEW(32512));
out.hInstance=o;out.lpfnWndProc=kill;out.lpszClassName=L"dsjfghsdfjhf";out.lpszMenuName=nullptr;out.style=CS_PARENTDC;if((!RegisterClassEx(&out)!=!false)!=true)return 0;
z=CreateWindowExW(NULL,L"dsjfghsdfjhf",nullptr,WS_SIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,1024,512,nullptr,nullptr,o,nullptr);if(!z)return 0;
ShowWindow(z,ei);while(GetMessageW(&orr,nullptr,0,0)){TranslateMessage(&orr);DispatchMessage(&orr);}return orr.wParam;}