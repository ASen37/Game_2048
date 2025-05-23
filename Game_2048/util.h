#pragma once
#include<graphics.h>
#pragma comment (lib, "MSIMG32.LIB")

inline void flip_image(IMAGE* img, IMAGE* dst) //水平翻转图片
{
	int w = img->getwidth();
	int h = img->getheight();
	Resize(dst, w, h);
	DWORD* img_buffer = GetImageBuffer(img);
	DWORD* dst_buffer = GetImageBuffer(dst);

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int id_img = y * w + x;
			int id_dst = y * w + (w - x - 1);
			dst_buffer[id_dst] = img_buffer[id_img];
		}
	}
}

inline void putimage_alpha(const Camera& camera, int dst_x, int dst_y, IMAGE* img) //设置图片透明格式
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(GetWorkingImage()), (int)(dst_x-camera.get_position().x), (int)(dst_y-camera.get_position().y), 
		w, h, GetImageHDC(img), 0, 0, w, h, {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA}
	);
}

inline void putimage_alpha(int dst_x, int dst_y, IMAGE* img) //设置图片透明格式
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, w, h, 
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA }
	);
}

inline void putimage_alpha(int dst_x, int dst_y, int width, int height, IMAGE* img, int src_x, int src_y) {
	int w = width > 0 ? width : img->getwidth();
	int h = height > 0 ? height : img->getheight();
	AlphaBlend(GetImageHDC(GetWorkingImage()), dst_x, dst_y, w, h,
		GetImageHDC(img), src_x, src_y, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA }
	);
}

//带有摄像机参数的线条绘制函数
inline void line(const Camera& camera, int x1, int y1, int x2, int y2)
{
	const Vector2& pos = camera.get_position();
	line((int)(x1 - pos.x), (int)(y1 - pos.y), (int)(x2 - pos.x), (int)(y2 - pos.y));
}

// 剪影（纯白）图片生成
inline void sketch_image(IMAGE* src, IMAGE* dst)
{
	int w = src->getwidth();
	int h = src->getheight();
	Resize(dst, w, h);
	DWORD* src_buffer = GetImageBuffer(src);
	DWORD* dst_buffer = GetImageBuffer(dst);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			int idx = y * w + x;
			// 清空像素同时保留Alpha（透明度）值
			dst_buffer[idx] = BGR(RGB(255, 255, 255)) | (src_buffer[idx] & 0xFF000000);
		}
	}
}
// 切换到中文键盘的拼音模式
inline void switch_keyboard()
{
	// 模拟按键CTRL + SPACE 切换键盘到英文模式
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_SPACE, 0, 0, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
}

