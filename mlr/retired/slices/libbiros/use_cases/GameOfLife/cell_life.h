#ifndef _CELL_LIFE_H_
#define _CELL_LIFE_H_

#include <biros/biros.h>
#include <Core/array.h>

class Cell;

class CellLife : public Process {
	public:
    CellLife() : Process("Cell Life Process") {}

    void open();
		void step();
		void close();

		Cell *own;
		mlr::Array<Cell*> neighbours;
};


#endif
