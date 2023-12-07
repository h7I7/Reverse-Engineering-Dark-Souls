#pragma once

class Modification
{
public:
	Modification()
		: Activated(false)
	{}

	virtual bool Update() = 0;
	virtual void Display() = 0;

protected:
	virtual void Setup() = 0;
	virtual void Toggle() = 0;

	bool Activated;
};