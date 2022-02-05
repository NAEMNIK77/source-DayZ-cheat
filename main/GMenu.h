#ifndef __GMENU__
#define __GMENU__

class GMenu {
	char szTitel[64];
	struct elements_t
	{
		char szName[64];
		int iMin;
		int iMax;
		int iStep;
		int *pValue;
		char szDescription[512];
	};

	struct sub_t {
		char szName[64];
		elements_t item[30];
		int items;
	};

	sub_t subitem[10];
	int iSelected;
	int x, y;
	int w;
	bool bOnHighlighting;
	bool bShowValue;

public:
	GMenu(char* szTitelName, int x, int y, int w, bool bInHighlighting, bool bShowValue);
	~GMenu();

	void AddEntry(char* szName, int* pValue, int iMin, int iMax, int iStep, char* desc = NULL);

	void AddEntry(sub_t* item, char* szName) {
		int count = item->items + 1;
		memcpy(&subitem[count], item, sizeof(sub_t));
		strcpy_s(subitem[count].szName, szName);
		subitem[count].items = count;
	}

	bool KeyHandling(int key);
	void Draw();
	void Do();
	elements_t list[100];
	bool bActive;
	bool bMenuActive;
	int iElements;
	bool boolAimkey;
	WNDPROC window;
}; 

#endif