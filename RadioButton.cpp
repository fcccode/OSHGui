#include "RadioButton.h"

namespace OSHGui
{
	//---------------------------------------------------------------------------
	//Constructor
	//---------------------------------------------------------------------------
	RadioButton::RadioButton(const std::shared_ptr<Control> &parent) : CheckBox(parent)
	{
		type = CONTROL_RADIOBUTTON;
		
		SetText(L"RadioButton");

		group = 0;
	}
	//---------------------------------------------------------------------------
	//Getter/Setter
	//---------------------------------------------------------------------------
	void RadioButton::SetChecked(bool checked)
	{
		if (this->checked != checked)
		{
			//uncheck other radiobuttons
			const std::vector<std::shared_ptr<Control>> &controls = Parent->GetControls();
			for (unsigned int i = 0; i < controls.size(); ++i)
			{
				std::shared_ptr<Control> control = controls.at(i);
				if (control->GetType() == CONTROL_RADIOBUTTON)
				{
					std::shared_ptr<RadioButton> &radio = std::static_pointer_cast<RadioButton>(control);
					if (radio->GetGroup() == group)
					{
						radio->SetCheckedInternal(false);
					}
				}
			}
			
			SetCheckedInternal(checked);
		}
	}
	//---------------------------------------------------------------------------
	void RadioButton::SetCheckedInternal(bool checked)
	{
		if (this->checked != checked)
		{
			this->checked = checked;
			
			changeEventHandler.Invoke(shared_from_this());
		}
	}
	//---------------------------------------------------------------------------
	void RadioButton::SetGroup(int group)
	{
		this->group = group;
	}
	//---------------------------------------------------------------------------
	int RadioButton::GetGroup()
	{
		return group;
	}
	//---------------------------------------------------------------------------
	//Event-Handling
	//---------------------------------------------------------------------------
	Event::NextEventTypes RadioButton::ProcessEvent(const std::shared_ptr<Event> &event)
	{
		if (event == 0)
		{
			return Event::DontContinue;
		}

		if (!visible || !enabled)
		{
			return Event::Continue;
		}
	
		if (event->Type == Event::Mouse)
		{
			std::shared_ptr<MouseEvent> &mouse = std::static_pointer_cast<MouseEvent>(event);
			Drawing::Point mousePositionBackup = mouse->Position;
			mouse->Position = PointToClient(mouse->Position);
			
			if (Drawing::Rectangle(0, 0, clientArea.GetWidth(), clientArea.GetHeight()).Contains(mouse->Position)) //ClientArea
			{
				if (mouse->State == MouseEvent::LeftDown)
				{
					pressed = true;
			
					if (!hasFocus)
					{
						Parent->RequestFocus(shared_from_this());
					}
					return Event::DontContinue;
				}
				else if (mouse->State == MouseEvent::LeftUp)
				{
					if (pressed && hasFocus)
					{
						SetChecked(true);
					
						pressed = false;
					}
					return Event::DontContinue;
				}
			}

			//restore PointToClient (alternatively call PointToScreen)
			mouse->Position = mousePositionBackup;
		}
		else if (event->Type == Event::Keyboard)
		{
			std::shared_ptr<KeyboardEvent> &keyboard = std::static_pointer_cast<KeyboardEvent>(event);
			if (keyboard->KeyCode == Key::Space)
			{
				SetChecked(!GetChecked());
				return Event::DontContinue;
			}
		}
		
		return Event::Continue;
	}
	//---------------------------------------------------------------------------
	void RadioButton::Render(const std::shared_ptr<Drawing::IRenderer> &renderer)
	{
		if (!visible)
		{
			return;
		}
		
		renderer->SetRenderColor(backColor);
		renderer->Fill(checkBoxPosition.Left, checkBoxPosition.Top, 17, 17);

		Drawing::Color white = Drawing::Color::White();
		renderer->SetRenderColor(white);
		renderer->FillGradient(checkBoxPosition.Left + 1, checkBoxPosition.Top + 1, 15, 15, white - Drawing::Color(0, 137, 137, 137));
		renderer->SetRenderColor(backColor);
		renderer->FillGradient(checkBoxPosition.Left + 2, checkBoxPosition.Top + 2, 13, 13, backColor + Drawing::Color(0, 55, 55, 55));
		
		if (checked)
		{
			renderer->Fill(checkBoxPosition.Left + 6, checkBoxPosition.Top + 4, 5, 9);
			renderer->Fill(checkBoxPosition.Left + 4, checkBoxPosition.Top + 6, 9, 5);
			renderer->Fill(checkBoxPosition.Left + 5, checkBoxPosition.Top + 5, 7, 7);
			
			renderer->SetRenderColor(white - Drawing::Color(0, 128, 128, 128));
			renderer->Fill(checkBoxPosition.Left + 5, checkBoxPosition.Top + 7, 7, 3);
			
			renderer->SetRenderColor(white);
			renderer->FillGradient(checkBoxPosition.Left + 7, checkBoxPosition.Top + 5, 3, 7, white - Drawing::Color(0, 137, 137, 137));
			renderer->FillGradient(checkBoxPosition.Left + 6, checkBoxPosition.Top + 6, 5, 5, white - Drawing::Color(0, 137, 137, 137));
		}

		renderer->SetRenderColor(foreColor);		
		renderer->RenderText(font, textPosition.Left, textPosition.Top, bounds.GetWidth() - 20, bounds.GetHeight(), textHelper.GetText());

		if (controls.size() > 0)
		{
			Drawing::Rectangle renderRect = renderer->GetRenderRectangle();
			renderer->SetRenderRectangle(clientArea + renderRect.GetPosition());
			
			for (unsigned int i = 0; i < controls.size(); ++i)
			{
				controls.at(i)->Render(renderer);
			}
			
			renderer->SetRenderRectangle(renderRect);
		}
	}
	//---------------------------------------------------------------------------
}