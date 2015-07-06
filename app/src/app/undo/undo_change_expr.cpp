#include <Python.h>

#include <QTextCursor>
#include <QPlainTextEdit>
#include <QGraphicsTextItem>

#include "app/undo/undo_change_expr.h"

#include "graph/datum.h"

UndoChangeExprCommand::UndoChangeExprCommand(
        Datum* d, QString before, QString after)
    : UndoChangeExprCommand(d, before, after, -1, -1, NULL)
{
    // Nothing to do here
}

UndoChangeExprCommand::UndoChangeExprCommand(
        Datum* d, QString before, QString after,
        int cursor_before, int cursor_after,
        QObject* obj)
    : d(d), before(before), after(after),
      cursor_before(cursor_before), cursor_after(cursor_after),
      obj(obj)
{
    setText("'set value'");
}


template<typename T>
void UndoChangeExprCommand::_saveCursor()
{
    if (auto txt = dynamic_cast<T*>(obj.data()))
        cursor_after = txt->textCursor().position();
}

template<typename T>
void UndoChangeExprCommand::_restoreCursor(int pos)
{
    if (auto txt = dynamic_cast<T*>(obj.data()))
    {
        QTextCursor c = txt->textCursor();
        c.setPosition(pos);
        txt->setTextCursor(c);
    }
}

void UndoChangeExprCommand::saveCursor()
{
    _saveCursor<QPlainTextEdit>();
    _saveCursor<QGraphicsTextItem>();
}

void UndoChangeExprCommand::restoreCursor(int pos)
{
    _restoreCursor<QPlainTextEdit>(pos);
    _restoreCursor<QGraphicsTextItem>(pos);
}

void UndoChangeExprCommand::redo()
{
    d->setText(after.toStdString());
    restoreCursor(cursor_after);
}

void UndoChangeExprCommand::undo()
{
    // Save text value and cursor position
    saveCursor();
    after = QString::fromStdString(d->getText());

    d->setText(before.toStdString());

    // Restore cursor to previous position
    restoreCursor(cursor_before);
}

void UndoChangeExprCommand::swapDatum(Datum* a, Datum* b) const
{
    if (d == a)
        d = b;
}
