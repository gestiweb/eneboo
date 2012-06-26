#include <qsinterpreter.h>
#include <qswrapperfactory.h>

#include <qapplication.h>
#include <qfile.h>
#include <qlistview.h>


/* Wrapper for QListView that gives access to the functions
 * palette() and setPalette();
 */
class ListViewWrapper : public QObject
{
    Q_OBJECT
public:
    ListViewWrapper(QListView *l) : listView(l) { }

public slots:
    QPalette palette() const { return listView->palette(); }
    void setPalette(const QPalette &palette) { listView->setPalette(palette); }

private:
    QListView *listView;
};


/* Wrapper for QListViewItem that gives access to the items text
 */
class ItemWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    ItemWrapper(QListViewItem *i) : item(i) { };

    QString text() const { return item->text(0); }
    void setText(const QString &txt) { item->setText(0, txt); }
private:
    QListViewItem *item;
};


class Wrappers : public QSWrapperFactory
{
public:
    Wrappers()
    {
	// Tell the factory what which classes we provide wrappers for.
	registerWrapper("QListView");
	registerWrapper("QListViewItem");
    }

    QObject *create(const QString &name, void *ptr)
    {
	if (name == "QListView")
	    return new ListViewWrapper((QListView *)ptr);
	else if (name == "QListViewItem")
	    return new ItemWrapper((QListViewItem *)ptr);
	return 0;
    }
};


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Get the script to load...
    QString fileName = "test.qs";
    if (argc > 1)
	fileName = argv[1];
    QFile file(fileName);
    if (!file.open(IO_ReadOnly)) {
	printf("Could not open file: %s, %s\n", fileName.latin1(),
#if QT_VERSION >= 0x030200
	       file.errorString().latin1()
#else
	       "Qt 3.2 will give you better info, you should upgrade ;-)"
#endif
	       );
	return 1;
    }
    QString script(file.readAll());

    // Create my list view
    QListView *listView = new QListView(0, "listView");
    listView->addColumn("1");
    new QListViewItem(listView, "Red");
    new QListViewItem(listView, "Blue");
    new QListViewItem(listView, "Green");
    new QListViewItem(listView, "Cyan");
    new QListViewItem(listView, "Yellow");
    new QListViewItem(listView, "Purple");
    new QListViewItem(listView, "Orange");
    new QListViewItem(listView, "Gray");

    // Create interpreter and add listview and factory to it. Order here is
    // important since adding a wrapper factory will reset the interpreter.
    QSInterpreter ip;
    ip.addWrapperFactory(new Wrappers());
    ip.addTransientObject(listView);

    ip.evaluate(script, 0, fileName);

    listView->show();

    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}

#include "wrappers.moc"
