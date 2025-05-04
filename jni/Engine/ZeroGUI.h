#ifndef Zero_Assert
#include <assert.h>
#define Zero_Assert(_EXPR)            assert(_EXPR)
#endif
#define Zero_ArraySize(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))


namespace ZeroGUI
{
	namespace Colors
	{
		FLinearColor Text{ 1.0f, 1.0f, 1.0f, 1.0f };
		FLinearColor Text_Shadow{ 0.0f, 0.0f, 0.0f, 0.0f };
		FLinearColor Text_Outline{ 0.0f, 0.0f, 0.0f, 0.30f };

		FLinearColor Window_Background{ 0.009f, 0.009f, 0.009f, 0.8f };
		FLinearColor Window_Header{ 0.10f, 0.15f, 0.84f, 0.8f };

		FLinearColor Button_Idle{ 0.10f, 0.15f, 0.84f, 1.0f };
		FLinearColor Button_Hovered{ 0.15f, 0.20f, 0.89f, 1.0f };
		FLinearColor Button_Active{ 0.20f, 0.25f, 0.94f, 1.0f };

		FLinearColor Checkbox_Idle{ 0.17f, 0.16f, 0.23f, 1.0f };
		FLinearColor Checkbox_Hovered{ 0.22f, 0.30f, 0.72f, 1.0f };
		FLinearColor Checkbox_Enabled{ 0.20f, 0.25f, 0.94f, 1.0f };

		FLinearColor Combobox_Idle{ 0.17f, 0.16f, 0.23f, 1.0f };
		FLinearColor Combobox_Hovered{ 0.17f, 0.16f, 0.23f, 1.0f };
		FLinearColor Combobox_Elements{ 0.239f, 0.42f, 0.82f, 1.0f };

		FLinearColor Slider_Idle{ 0.17f, 0.16f, 0.23f, 1.0f };
		FLinearColor Slider_Hovered{ 0.17f, 0.16f, 0.23f, 1.0f };
		FLinearColor Slider_Progress{ 0.22f, 0.30f, 0.72f, 1.0f };
		FLinearColor Slider_Button{ 0.10f, 0.15f, 0.84f, 1.0f };

		FLinearColor ColorPicker_Background{ 0.006f, 0.006f, 0.006f, 1.0f };
		
        FLinearColor 白色 = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
        FLinearColor 灰色 = FLinearColor(0.15f, 0.15f, 0.15f, 1.0f);
        FLinearColor 红色 = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
        FLinearColor 黑色 = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
        FLinearColor 黄色 = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f);
        FLinearColor 浅蓝色 = FLinearColor(0.678f, 0.847f, 0.902f, 1.0f);
        FLinearColor 绿色 = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
        FLinearColor 透明红色 = FLinearColor(1.0f, 0.0f, 0.0f, 0.5f);
        FLinearColor 草绿色 = FLinearColor(0.196f, 0.804f, 0.196f, 1.0f);		
	}

	void* canvas;
	void* font;
	
	bool hover_element = false;
	Vector2A menu_pos = Vector2A{ 0, 0 };
	float offset_x = 0.0f;
	float offset_y = 0.0f;

	Vector2A first_element_pos = Vector2A{ 0, 0 };

	Vector2A last_element_pos = Vector2A{ 0, 0 };
	Vector2A last_element_size = Vector2A{ 0, 0 };

	int current_element = -1;
	Vector2A current_element_pos = Vector2A{ 0, 0 };
	Vector2A current_element_size = Vector2A{ 0, 0 };
	int elements_count = 0;
	bool sameLine = false;

	bool pushY = false;
	float pushYvalue = 0.0f;

	bool mouseDownAlready[256];
    Vector2A MousePos = {0,0};
    bool MouseDown = false;

    bool IsMouseClicked(int button, int element_id, bool repeat)
    {
        if (MouseDown)
        {
            if (!mouseDownAlready[element_id])
            {
                mouseDownAlready[element_id] = true;
                return true;
            }
            if (repeat)
                return true;
        }
        else
        {
            mouseDownAlready[element_id] = false;
        }
        return false;
	}

	void SetupCanvas(void* _canvas, void* _font)
	{
		canvas = _canvas;
		font = _font;
	}

	Vector2A CursorPos()
    {
        return MousePos;
	}
	bool MouseInZone(Vector2A pos, Vector2A size)
    {
        Vector2A cursor_pos = CursorPos();
        if (cursor_pos.X > pos.X && cursor_pos.Y > pos.Y)
            if (cursor_pos.X < pos.X + size.X && cursor_pos.Y < pos.Y + size.Y)
                return true;

        return false;
	}

	void Draw_Cursor(bool toogle)
	{
		if (toogle)
		{
			Vector2A cursorPos = CursorPos();
		    K2_DrawLine(canvas,Vector2A{ cursorPos.X, cursorPos.Y }, Vector2A{ cursorPos.X + 35, cursorPos.Y + 10 }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });


			int x = 35;
			int y = 10;
			while (y != 30) //20 steps
			{
				x -= 1; if (x < 15) x = 15;
				y += 1; if (y > 30) y = 30;

				K2_DrawLine(canvas,Vector2A{ cursorPos.X, cursorPos.Y }, Vector2A{ cursorPos.X + x, cursorPos.Y + y }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });
			}

			K2_DrawLine(canvas,Vector2A{ cursorPos.X, cursorPos.Y }, Vector2A{ cursorPos.X + 15, cursorPos.Y + 30 }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });
			K2_DrawLine(canvas,Vector2A{ cursorPos.X + 35, cursorPos.Y + 10 }, Vector2A{ cursorPos.X + 15, cursorPos.Y + 30 }, 1, FLinearColor{ 0.30f, 0.30f, 0.80f, 1.0f });
		}
	}

	void SameLine()
	{
		sameLine = true;
	}
	void PushNextElementY(float y, bool from_last_element = true)
	{
		pushY = true;
		if (from_last_element)
			pushYvalue = last_element_pos.Y + last_element_size.Y + y;
		else
			pushYvalue = y;
	}
	void NextColumn(float x)
	{
		offset_x = x;
		PushNextElementY(first_element_pos.Y, false);
	}
	
	void TextLeft(const char* name, Vector2A pos, FLinearColor color, bool outline)
	{
		int length = strlen(name) + 1;
		K2_DrawText(canvas,font, FString(name),pos, color, false, Colors::Text_Shadow, Vector2A{ pos.X + 1, pos.Y + 1 }, false, true, true, Colors::Text_Outline);

	}
	void TextCenter(const char* name, Vector2A pos, FLinearColor color, bool outline)
	{
		int length = strlen(name) + 1;
		K2_DrawText(canvas,font, FString(name),pos, color, false, Colors::Text_Shadow, Vector2A{ pos.X + 1, pos.Y + 1 }, true, true, true, Colors::Text_Outline);
	}
	
    void TextOutlined(const char* Text, Vector2A Pos, FLinearColor Color, FLinearColor OutlineColor, bool isCenter = false)
    {
		int length = strlen(Text) + 1;    
        K2_DrawText(canvas, font, FString(Text), Pos, Color, 1.f, {}, {}, isCenter, isCenter, true, OutlineColor);
    }

	void GetColor(FLinearColor* color, float* r, float* g, float* b, float* a)
	{
		*r = color->R;
		*g = color->G;
		*b = color->B;
		*a = color->A;
	}

    static void* RenderTexture = nullptr;
    void Draw_Texture(Vector2A Pos, Vector2A size, FLinearColor Color)
    {
        int Max = 6; // EBlendMode::Max
        K2_DrawTexture(canvas, RenderTexture, Pos, size, {}, {}, Color, Max, 0, {});
    }
	void Draw_Line(Vector2A from, Vector2A to, int thickness, FLinearColor color)
	{
		K2_DrawLine(canvas,Vector2A{ from.X, from.Y }, Vector2A{ to.X, to.Y }, thickness, color);
	}
	void DrawFilledRect(Vector2A initial_pos, float w, float h, FLinearColor color)
	{
//		for (float i = 0.0f; i < h; i += 1.0f)
//		K2_DrawLine(canvas,Vector2A{ initial_pos.X, initial_pos.Y + i }, Vector2A{ initial_pos.X + w, initial_pos.Y + i }, 1.0f, color);
        Vector2A size;
        size.X = w;
        size.Y = h;
        Draw_Texture(initial_pos, size, color);
	}
    void DrawRectangle(Vector2A Pos, float Width, float Height, float Thickness, FLinearColor Color) {
    	K2_DrawLine(canvas,Vector2A(Pos.X, Pos.Y), Vector2A(Pos.X + Width, Pos.Y), Thickness, Color);
    	K2_DrawLine(canvas,Vector2A(Pos.X, Pos.Y), Vector2A(Pos.X, Pos.Y + Height), Thickness, Color);
    	K2_DrawLine(canvas,Vector2A(Pos.X + Width, Pos.Y), Vector2A(Pos.X + Width, Pos.Y + Height), Thickness, Color);
    	K2_DrawLine(canvas,Vector2A(Pos.X, Pos.Y + Height), Vector2A(Pos.X + Width, Pos.Y + Height), Thickness, Color);
    }	
	void DrawFilledCircle(Vector2A pos, float r, FLinearColor color)
	{
		float smooth = 0.07f;

		double PI = 3.14159265359;
		int size = (int)(2.0f * PI / smooth) + 1;

		float angle = 0.0f;
		int i = 0;

		for (; angle < 2 * PI; angle += smooth, i++)
		{
			Draw_Line(Vector2A{ pos.X, pos.Y }, Vector2A{ pos.X + cosf(angle) * r, pos.Y + sinf(angle) * r }, 1.0f, color);
		}
	}
    void DrawCircle(Vector2A pos, int radius, int numSides, FLinearColor Color)
    {
        static const double PI = 3.14159265359;
        float Step = PI * 2.0 / numSides;
    
        float cosStep = cos(Step);
        float sinStep = sin(Step);
        float X1 = radius;
        float Y1 = 0.0f;
    
        for (int Count = 0; Count < numSides; ++Count) {
            float X2 = radius * cos(Step * (Count + 1));
            float Y2 = radius * sin(Step * (Count + 1));
            Draw_Line(Vector2A{pos.X + X1, pos.Y + Y1}, Vector2A{pos.X + X2, pos.Y + Y2}, 1.0f, Color);
            X1 = X2;
            Y1 = Y2;
        }
    }

	Vector2A dragPos;
	Vector2A movePos;
	Vector2A origPos;
	bool Window(char* name, Vector2A* pos, Vector2A size, bool& isOpen, float& tempValue)
	{
		elements_count = 0;

		bool isHovered = MouseInZone(Vector2A{ pos->X, pos->Y }, isOpen ? size : Vector2A(size.X, 25.0f));

		//Drop last element
		if (current_element != -1 && !MouseDown)
		{
			current_element = -1;
		}

		//Drag
		if (hover_element && MouseDown)
		{

		}
		else if ((isHovered || dragPos.X != 0) && !hover_element)
		{
			if (IsMouseClicked(0, elements_count, true))
			{
				Vector2A cursorPos = CursorPos();

				cursorPos.X -= size.X;
				cursorPos.Y -= size.Y;

				if (dragPos.X == 0)
				{
					dragPos.X = (cursorPos.X - pos->X);
					dragPos.Y = (cursorPos.Y - pos->Y);
					origPos = {cursorPos.X - dragPos.X, cursorPos.Y - dragPos.Y};
					// 判断点击点是否在标题栏内
    		        bool isClickedTitle = MouseInZone(Vector2A{ pos->X, pos->Y }, {size.X, 25.0f});
    		        if(isClickedTitle){
    		            tempValue = 1.0f;
    		        }else{
    		            tempValue = 0.0f;
    		        }
				}
				
				
				pos->X = cursorPos.X - dragPos.X;
				pos->Y = cursorPos.Y - dragPos.Y;
				
			
				
			}
			else
			{
			
				dragPos = Vector2A{ 0, 0 };
				if(tempValue && abs(origPos.X - pos->X) <= 10 && abs(origPos.Y - pos->Y) <= 10){
				    tempValue = 0;
				    isOpen = !isOpen;
				}
			}
		}
		else
		{
			hover_element = false;
		}


		offset_x = 0.0f; offset_y = 0.0f;
		menu_pos = Vector2A{ pos->X, pos->Y };
		first_element_pos = Vector2A{ 0, 0 };
		current_element_pos = Vector2A{ 0, 0 };
		current_element_size = Vector2A{ 0, 0 };
	    
	    if (isOpen)
	        // Bg
	        Draw_Texture(Vector2A{ pos->X, pos->Y }, Vector2A{ size.X, size.Y}, Colors::Window_Background);
//	        DrawFilledRect(Vector2A{ pos->X, pos->Y }, size.X, size.Y, Colors::Window_Background);

	    
	    //Header
	        Draw_Texture(Vector2A{ pos->X, pos->Y }, Vector2A{ size.X, 25.0f }, Colors::Window_Header);
//    		DrawFilledRect(Vector2A{ pos->X, pos->Y }, size.X, 25.0f, Colors::Window_Header);

		offset_y += 25.0f;

		//Title
		Vector2A titlePos = Vector2A{ pos->X + size.X / 2, pos->Y + 25 / 2 };
		TextCenter(name, titlePos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
		
	
		//DrawFilledRect(Vector2A{ pos->X, pos->Y }, 122, size.Y, FLinearColor{ 0.006f, 0.006f, 0.006f, 1.0f });//My tabs bg

	
        

		return isOpen;
	}
	
	void Text(const char* text, bool center = false, bool outline = false)
	{
		elements_count++;

		float size = 12.5;
		Vector2A padding = Vector2A{ 10, 10 };
		Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}

		if (!sameLine)
			offset_y += size + padding.Y;

		//Text
		Vector2A textPos = Vector2A{ pos.X + 5.0f, pos.Y + size / 2 };
		if (center)
			TextCenter(text, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, outline);
		else
			TextLeft(text, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, outline);

		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
	}
	bool ButtonTab(const char* name, Vector2A size, bool active)
	{
		elements_count++;

		Vector2A padding = Vector2A{ 5, 10 };
		Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2A{ pos.X, pos.Y }, size);

		//Bg
		if (active)
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Active);
		}
		else if (isHovered)
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Idle);
		}

		if (!sameLine)
			offset_y += size.Y + padding.Y;

		//Text
		Vector2A textPos = Vector2A{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
		TextCenter(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;

		if (isHovered && IsMouseClicked(0, elements_count, false))
			return true;

		return false;
	}
	bool Button(const char* name, Vector2A size)
	{
		elements_count++;
		
		Vector2A padding = Vector2A{ 5, 10 };
		Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2A{ pos.X, pos.Y }, size);
		
		//Bg
		if (isHovered)
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Hovered);
			hover_element = true;
		}
		else
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Button_Idle);
		}
		
		if (!sameLine)
			offset_y += size.Y + padding.Y;
		
		//Text
		Vector2A textPos = Vector2A{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
		//if (!TextOverlapedFromActiveElement(textPos))
			TextCenter(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
		
		
		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
		
		if (isHovered && IsMouseClicked(0, elements_count, false))
			return true;
		
		return false;
	}
	bool Checkbox(const char* name, bool* value)
	{
		elements_count++;
		
		float size = 30;
		Vector2A padding = Vector2A{ 10, 10 };
		Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2A{ pos.X, pos.Y }, Vector2A{ size, size });
		
		//Bg
		/*if (isHovered)
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size, size, Colors::Checkbox_Hovered);
			hover_element = true;
		}*/
		//else
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y }, size, size, Colors::Checkbox_Idle);
		}
	
		if (!sameLine)
			offset_y += size + padding.Y;
	
		if (*value)
		{
			DrawFilledRect(Vector2A{ pos.X + 3, pos.Y + 3 }, size - 6, size - 6, Colors::Checkbox_Enabled);
			//DrawFilledRect(Vector2A{ pos.X + 9, pos.Y + 9 }, size - 18, size - 18, Colors::Checkbox_Hovered);
		}
		
		
		
		//Text
		Vector2A textPos = Vector2A{ pos.X + size + 5.0f, pos.Y + size / 2 };
		//if (!TextOverlapedFromActiveElement(textPos))
			TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
		
		
		sameLine = false;
		last_element_pos = pos;
		//last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
		
		if (isHovered && IsMouseClicked(0, elements_count, false)){
			*value = !*value;
			return true;
        }
        return false;
	}
	bool SliderInt(const char* name, int* value, int min, int max, const char* format="%i")
	{
		elements_count++;

		Vector2A size = Vector2A{ 240, 50 };
		Vector2A slider_size = Vector2A{ 200, 10 };
		Vector2A padding = Vector2A{ 10, 15 };
		Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size);

		if (!sameLine)
			offset_y += size.Y + padding.Y;
        bool change = false;
		//Bg
		if (isHovered || current_element == elements_count)
		{
			//Drag
			if (IsMouseClicked(0, elements_count, true))
			{
				current_element = elements_count;

				Vector2A cursorPos = CursorPos();
				*value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
				if (*value < min) *value = min;
				if (*value > max) *value = max;
				change = true;
			}

			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Hovered);
			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f }, 5.0f, 5.0f, Colors::Slider_Progress);

			hover_element = true;
		}
		else
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Idle);
			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f }, 5.0f, 5.0f, Colors::Slider_Progress);
		}


		//Value
		float oneP = slider_size.X / (max - min);
		DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, oneP * (*value - min), slider_size.Y, Colors::Slider_Progress);
		//DrawFilledRect(Vector2A{ pos.X + oneP * (*value - min) - 10.0f, pos.Y + slider_size.Y - 5.0f + padding.Y }, 20.0f, 20.0f, Colors::Slider_Button);
		DrawFilledCircle(Vector2A{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y }, 10.0f, Colors::Slider_Button);
		DrawFilledCircle(Vector2A{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y }, 5.0f, Colors::Slider_Progress);

		char buffer[32];
		sprintf(buffer, format, *value);
		Vector2A valuePos = Vector2A{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 25 + padding.Y };
		TextCenter(buffer, valuePos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);

		//Text
		Vector2A textPos = Vector2A{ pos.X + 5, pos.Y + 10 };
		TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
		return change;
	}
	bool SliderFloat(const char* name, float* value, float min, float max, const char* format = "%.0f")
	{
		elements_count++;

		Vector2A size = Vector2A{ 240, 50 };
		Vector2A slider_size = Vector2A{ 200, 10 };
		Vector2A padding = Vector2A{ 10, 15 };
		Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
		if (sameLine)
		{
			pos.X = last_element_pos.X + last_element_size.X + padding.X;
			pos.Y = last_element_pos.Y;
		}
		if (pushY)
		{
			pos.Y = pushYvalue;
			pushY = false;
			pushYvalue = 0.0f;
			offset_y = pos.Y - menu_pos.Y;
		}
		bool isHovered = MouseInZone(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size);

		if (!sameLine)
			offset_y += size.Y + padding.Y;
        bool change = false;
		//Bg
		if (isHovered || current_element == elements_count)
		{
			//Drag
			if (IsMouseClicked(0, elements_count, true))
			{
				current_element = elements_count;

				Vector2A cursorPos = CursorPos();
				*value = ((cursorPos.X - pos.X) * ((max - min) / slider_size.X)) + min;
				if (*value < min) *value = min;
				if (*value > max) *value = max;
				change = true;
			}

			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Hovered);
			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f }, 5.0f, 5.0f, Colors::Slider_Progress);

			hover_element = true;
		}
		else
		{
			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, slider_size.X, slider_size.Y, Colors::Slider_Idle);
			DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y + 5.0f }, 5.0f, 5.0f, Colors::Slider_Progress);
		}


		//Text
		Vector2A textPos = Vector2A{ pos.X, pos.Y + 5 };
		TextLeft(name, textPos, Colors::Text, false);

		//Value
		float oneP = slider_size.X / (max - min);
		DrawFilledRect(Vector2A{ pos.X, pos.Y + slider_size.Y + padding.Y }, oneP * (*value - min), slider_size.Y, Colors::Slider_Progress);
		DrawFilledCircle(Vector2A{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y }, 10.0f, Colors::Slider_Button);
		DrawFilledCircle(Vector2A{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 3.3f + padding.Y }, 5.0f, Colors::Slider_Progress);

		char buffer[64];
		sprintf(buffer, format, *value);
		Vector2A valuePos = Vector2A{ pos.X + oneP * (*value - min), pos.Y + slider_size.Y + 20 + padding.Y };
		TextCenter(buffer, valuePos, Colors::Text, false);


		sameLine = false;
		last_element_pos = pos;
		last_element_size = size;
		if (first_element_pos.X == 0.0f)
			first_element_pos = pos;
		return change;
	}

    
    bool checkbox_enabled[256];
    void Combobox(char* name, Vector2A size, int* value, const char* arg, ...)
    {
    	elements_count++;
    
    	Vector2A padding = Vector2A{ 5, 10 };
    	Vector2A pos = Vector2A{ menu_pos.X + padding.X + offset_x, menu_pos.Y + padding.Y + offset_y };
    	if (sameLine)
    	{
    		pos.X = last_element_pos.X + last_element_size.X + padding.X;
    		pos.Y = last_element_pos.Y;
    	}
    	if (pushY)
    	{
    		pos.Y = pushYvalue;
    		pushY = false;
    		pushYvalue = 0.0f;
    		offset_y = pos.Y - menu_pos.Y;
    	}
    	bool isHovered = MouseInZone(Vector2A{ pos.X, pos.Y }, size);
    
    	//Bg
    	if (isHovered || checkbox_enabled[elements_count])
    	{
    		DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Combobox_Hovered);
    
    		hover_element = true;
    	}
    	else
    	{
    		DrawFilledRect(Vector2A{ pos.X, pos.Y }, size.X, size.Y, Colors::Combobox_Idle);
    	}
    
    	if (!sameLine)
    		offset_y += size.Y + padding.Y;
    
    	//Text
    	Vector2A textPos = Vector2A{ pos.X + size.X + 5.0f, pos.Y + size.Y / 2 };
    	TextLeft(name, textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
    
    	//Elements
    	bool isHovered2 = false;
    	Vector2A element_pos = pos;
    	int num = 0;
    
    	if (checkbox_enabled[elements_count])
    	{
    		current_element_size.X = element_pos.X - 5.0f;
    		current_element_size.Y = element_pos.Y - 5.0f;
    	}
    	va_list arguments;
    	for (va_start(arguments, arg); arg != NULL; arg = va_arg(arguments, const char*))
    	{
    		//Selected Element
    		if (num == *value)
    		{
    			Vector2A _textPos = Vector2A{ pos.X + size.X / 2, pos.Y + size.Y / 2 };
    			TextCenter((char*)arg, _textPos, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
    		}
    
    		if (checkbox_enabled[elements_count])
    		{
    			element_pos.Y += 25.0f;
    
    			isHovered2 = MouseInZone(Vector2A{ element_pos.X, element_pos.Y }, Vector2A{ size.X, 25.0f });
    			if (isHovered2)
    			{
    				hover_element = true;
    				DrawFilledRect(Vector2A{ element_pos.X, element_pos.Y }, size.X, 25.0f, Colors::Combobox_Hovered);
    
    				//Click
    				if (IsMouseClicked(0, elements_count, false))
    				{
    					*value = num;
    					checkbox_enabled[elements_count] = false;
    				}
    			}
    			else
    			{
    				DrawFilledRect(Vector2A{ element_pos.X, element_pos.Y }, size.X, 25.0f, Colors::Combobox_Idle);
    			}
    
    			TextLeft((char*)arg, Vector2A{ element_pos.X + 5.0f, element_pos.Y + 15.0f }, FLinearColor{ 1.0f, 1.0f, 1.0f, 1.0f }, false);
    		}
    		num++;
    	}
    	va_end(arguments);
    	if (checkbox_enabled[elements_count])
    	{
    		current_element_size.X = element_pos.X + 5.0f;
    		current_element_size.Y = element_pos.Y + 5.0f;
    	}
    
    
    	sameLine = false;
    	last_element_pos = pos;
    	last_element_size = size;
    	if (first_element_pos.X == 0.0f)
    		first_element_pos = pos;
    
    	if (isHovered && IsMouseClicked(0, elements_count, false))
    	{
    		checkbox_enabled[elements_count] = !checkbox_enabled[elements_count];
    	}
    	if (!isHovered && !isHovered2 && IsMouseClicked(0, elements_count, false))
    	{
    		checkbox_enabled[elements_count] = false;
    	}
    }

	void onEvent(AInputEvent* input_event,Vector2A screen_scale) {
     auto event_type = AInputEvent_getType(input_event);
     if(event_type != AINPUT_EVENT_TYPE_MOTION)
       return; // 仅处理触摸事件
     int32_t event_action = AMotionEvent_getAction(input_event);
     int32_t event_pointer_index = (event_action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
     if(event_pointer_index > 0)
       return; // 单指
       event_action &= AMOTION_EVENT_ACTION_MASK;
       switch (event_action) {
          case AMOTION_EVENT_ACTION_DOWN:
          case AMOTION_EVENT_ACTION_UP:
          if ((AMotionEvent_getToolType(input_event, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_FINGER) || (AMotionEvent_getToolType(input_event, event_pointer_index) == AMOTION_EVENT_TOOL_TYPE_UNKNOWN)) {
              MouseDown = (event_action == AMOTION_EVENT_ACTION_DOWN);
              Vector2A pos(AMotionEvent_getRawX(input_event, event_pointer_index), AMotionEvent_getRawY(input_event, event_pointer_index));
              MousePos = Vector2A(screen_scale.X > 0 ? pos.X / screen_scale.X : pos.X, screen_scale.Y > 0 ? pos.Y / screen_scale.Y : pos.Y);
          }
            break;
          case AMOTION_EVENT_ACTION_BUTTON_PRESS:
          case AMOTION_EVENT_ACTION_BUTTON_RELEASE: {
                int32_t button_state = AMotionEvent_getButtonState(input_event);
                MouseDown = ((button_state & AMOTION_EVENT_BUTTON_PRIMARY) != 0);

          }
            break;
          case AMOTION_EVENT_ACTION_HOVER_MOVE: // Hovering: Tool moves while NOT pressed (such as a physical mouse)
          case AMOTION_EVENT_ACTION_MOVE: {       // Touch pointer moves while DOWN
                Vector2A pos(AMotionEvent_getRawX(input_event, event_pointer_index), AMotionEvent_getRawY(input_event, event_pointer_index));
                MousePos = Vector2A(screen_scale.X > 0 ? pos.X / screen_scale.X : pos.X, screen_scale.Y > 0 ? pos.Y / screen_scale.Y : pos.Y);
                break;
          }
 
       default:
         break;
   }
}

};