#ifndef CONSOLE_H
#define CONSOLE_H

class Console
{
	enum ConsoleParam { NUM_LINES, NUM_COLUMNS, NUM_VISIBLE_LINES };

public:
	virtual ~Console ();

	int SetNumberOfLines (int lines);
	int SetNumberOfColumns (int columns);
	int SetNumVisibleLines (int lines);

	void RedirectToConsole ();
	bool SetConsoleParam (ConsoleParam param);
	int  GetConsoleParam (ConsoleParam param);

	bool AllocateConsole ();
	void RemoveConsole ();

	static Console* Allocate ()
	{
		if (!instance)
		{
			instance = new Console;
		}
		return instance;
	}
	static Console* instance;

protected:
	Console  ();

private:
	int numColumns;
	int numLines;
	int numVisibleLines;


};

#endif
