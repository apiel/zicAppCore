#ifndef APP_CORE_VIEW_TABLE_H_
#define APP_CORE_VIEW_TABLE_H_

#include "./app_core_renderer.h"
#include "./app_core_view.h"

#ifndef TABLE_VISIBLE_ROWS
#define TABLE_VISIBLE_ROWS 10
#endif

class App_View_TableField {
public:
    virtual void render(App_Renderer* renderer, uint8_t row, uint8_t col, uint8_t selectedRow, uint8_t selectedCol) = 0;

    virtual uint8_t update(UiKeys* keys, App_Renderer* renderer, uint8_t row, uint8_t col)
    {
        return VIEW_NONE;
    }

    virtual void updateEnd(uint8_t row, uint8_t col)
    {
    }

    virtual void updateStart(uint8_t row, uint8_t col)
    {
    }

    virtual bool isSelectable(uint8_t row, uint8_t col)
    {
        return false;
    }

    virtual void selected(App_Renderer* renderer, uint8_t row, uint8_t col)
    {
    }
};

class App_View_TableFieldCursor : public App_View_TableField {
protected:
    uint8_t cursorLen;

public:
    App_View_TableFieldCursor(uint8_t _cursorLen)
        : cursorLen(_cursorLen)
    {
    }

    bool isSelectable(uint8_t row, uint8_t col) override
    {
        return true;
    }

    void selected(App_Renderer* renderer, uint8_t row, uint8_t col) override
    {
        renderer->setCursor(cursorLen);
    }
};

class App_View_TableLabeledRow : public App_View_TableFieldCursor {
protected:
    const char* label;

public:
    App_View_TableLabeledRow(const char* _label, uint8_t _cursorLen)
        : App_View_TableFieldCursor(_cursorLen)
        , label(_label)
    {
    }

    virtual void renderValue(App_Renderer* renderer, uint8_t col) = 0;

    bool isSelectable(uint8_t row, uint8_t col) override
    {
        return col != 0;
    }

    void render(App_Renderer* renderer, uint8_t row, uint8_t col, uint8_t selectedRow, uint8_t selectedCol)
    {
        if (col == 0) {
            strcat(renderer->text, label);
        } else {
            renderValue(renderer, col);
        }
    }
};

class App_View_Table : public App_View {
protected:
    const uint8_t ROW_COUNT;
    uint8_t lastRow;
    const uint8_t COL_COUNT;
    App_View_TableField** fields;
    bool editingField = false;

    App_View_TableField* getSelectedField()
    {
        return fields[selectedRow * COL_COUNT + selectedCol];
    }

    void selectNextRow(int8_t direction)
    {
        uint8_t start = selectedRow;
        do {
            selectedRow = (selectedRow + lastRow + direction) % lastRow;
            if (getSelectedField() && getSelectedField()->isSelectable(selectedRow, selectedCol)) {
                onSelectRow();
                break;
            }
        } while (selectedRow != start);
    }

    void selectNextCol(int8_t direction)
    {
        if (selectedCol + direction > -1 && selectedCol + direction < COL_COUNT) {
            uint8_t start = selectedCol;
            while (1) {
                selectedCol = selectedCol + direction;
                if (selectedCol < 0 || selectedCol >= COL_COUNT) {
                    break;
                }
                if (getSelectedField() && getSelectedField()->isSelectable(selectedRow, selectedCol)) {
                    onSelectCol();
                    return;
                }
            }
            selectedCol = start;
        }
        onNextColOverflow(direction);
    }

    virtual void onSelectRow() { }
    virtual void onSelectCol() { }
    virtual void onNextColOverflow(int8_t direction) { }

public:
    uint8_t selectedRow = 0;
    uint8_t selectedCol = 0;

    App_View_Table(App_View_TableField** _fields, uint8_t _row, uint8_t _col)
        : ROW_COUNT(_row)
        , lastRow(_row)
        , COL_COUNT(_col)
        , fields(_fields)
    {
        // initSelection();
    }

    void initSelection()
    {
        for (uint8_t row = 0; row < ROW_COUNT; row++) {
            for (uint8_t col = 0; col < COL_COUNT; col++) {
                App_View_TableField* field = fields[row * COL_COUNT + col];
                if (field != NULL && field->isSelectable(row, col)) {
                    selectedRow = row;
                    selectedCol = col;
                    return;
                }
            }
        }
    }

    void setLastRow(uint8_t _lastRow)
    {
        lastRow = _lastRow > ROW_COUNT ? ROW_COUNT : _lastRow;
    }

    virtual void preRender(App_Renderer* renderer)
    {
        strcpy(renderer->text, "");
    }

    virtual void endRow(App_Renderer* renderer, uint16_t row)
    {
        strcat(renderer->text, "\n");
    }

    virtual void render(App_Renderer* renderer)
    {
        preRender(renderer);
        for (uint8_t row = renderer->startRow; row < lastRow && row - renderer->startRow < TABLE_VISIBLE_ROWS; row++) {
            // here would come if visible row
            for (uint8_t col = 0; col < COL_COUNT; col++) {
                // here would come if visible col
                App_View_TableField* field = fields[row * COL_COUNT + col];
                if (field != NULL) {
                    if (selectedRow == row && selectedCol == col) {
                        field->selected(renderer, row, col);
                    }
                    field->render(renderer, row, col, selectedRow, selectedCol);
                }
            }
            endRow(renderer, row);
        }
    }

    virtual uint8_t update(UiKeys* keys, App_Renderer* renderer)
    {
        uint8_t res = VIEW_CHANGED;
        if (keys->Edit) {
            if (!editingField) {
                getSelectedField()->updateStart(selectedRow, selectedCol);
                editingField = true;
            }
            res = getSelectedField()->update(keys, renderer, selectedRow, selectedCol);
        } else {
            if (editingField) {
                getSelectedField()->updateEnd(selectedRow, selectedCol);
                editingField = false;
            }
            if (keys->Up) {
                selectNextRow(-1);
            } else if (keys->Down) {
                selectNextRow(+1);
            } else if (keys->Left) {
                selectNextCol(-1);
            } else if (keys->Right) {
                selectNextCol(+1);
            }

            if (selectedRow < TABLE_VISIBLE_ROWS * 0.5) {
                renderer->startRow = 0;
            } else if (selectedRow > renderer->startRow + TABLE_VISIBLE_ROWS - 1) {
                renderer->startRow = selectedRow - TABLE_VISIBLE_ROWS + 1;
            } else if (selectedRow < renderer->startRow) {
                renderer->startRow = selectedRow;
            }
        }
        if (res != VIEW_NONE) {
            render(renderer);
        }
        return res;
    }
};

#endif