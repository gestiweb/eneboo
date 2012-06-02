#include <qsinterpreter.h>
#include <qsobjectfactory.h>
#include <qsproject.h>
#include <qsworkbench.h>
#include <qapplication.h>

/* Below is a simple example that demonstrates how enums can be exported to
 * QSA. The is trivial and has no purpose other than to demonstrate the
 * required steps.
 *
 */

/* Provide an object that will represent the static part of the Direction class,
 * allowing scripters to write contructs like:
 *
 * var x = Direction.LeftToRight;
 *
 * The key is the Q_ENUMS() declaration which exports the enum Mode via through
 * moc, which makes it available to QSA.
 */
class DirectionStatic : public QObject
{
    Q_OBJECT
    Q_OVERRIDE(QString name SCRIPTABLE false)
    Q_ENUMS(Mode)
public:
    enum Mode { Undefined, LeftToRight, RightToLeft };
};

/* Proved the class that will represent the instances in the script. Notice
 * that this class redeclares the enum mode so that it will be possible
 * to write constructions like:
 *
 * var x = dir.LeftToRight;
 *
 * where 'x' is an instance of the direction class.
 */
class Direction : public QObject
{
    Q_OBJECT
    Q_OVERRIDE(QString name SCRIPTABLE false)
    Q_ENUMS(Mode)
public:
    enum Mode { Undefined, LeftToRight, RightToLeft };

    Direction(QSInterpreter *interp) : d(Undefined), ip(interp) { };

public slots:
    bool isLeftToRight() const { return d == LeftToRight; }
    bool isRightToLeft() const { return d == RightToLeft; }

    void setMode(int mode) {
        if (mode == LeftToRight || mode == RightToLeft)
            d = Mode(mode);
        else
            ip->throwError("mode must be Direction.LeftToRight or Direction.RightToLeft");
    }

private:
    Mode d;
    QSInterpreter *ip;
};

class Factory : public QSObjectFactory
{
public:
    Factory()
    {
        registerClass("Direction", new DirectionStatic);
    }

    QObject *create(const QString &, const QSArgumentList &, QObject *)
    {
        return new Direction(interpreter());
    }
};


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QSProject p;
    p.load("enums.qsa");
    QSWorkbench wb(&p);

    p.interpreter()->addObjectFactory(new Factory);

    wb.open();

    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    QObject::connect(&p, SIGNAL(projectEvaluated()), &p, SLOT(save()));

    app.exec();
}

#include "main.moc"
