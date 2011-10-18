
extern QuickInterpreter *get_quick_interpreter(QSInterpreter*);

void QSVariablesContainer::updateScope()
{
#if QT_VERSION > 0x030100
    QSProject *pro = ::qt_cast<QSProject*>(QObject::sender());
#else
    const QObject *obj = QObject::sender();
    const QSProject *pro = obj ? (obj->inherits("QSProject") ? (QSProject*)obj : 0) : 0;
#endif
    updateScope(pro->interpreter());
}

void QSVariablesContainer::updateScope(QSInterpreter *interpreter)
{
    // get scope
    QSEnv *env = get_quick_interpreter(interpreter)->env();

    // build tree
    // FIXME: list all the added objects too
    variablesListView->clear();
    int level = 0;
    QSObject scope = env->scopeObjectAt(level++);
    while ( scope.isValid() ) {
	QListViewItem *item = new QListViewItem(variablesListView, scope.objectType()->identifier());
	updateMembers(item, &scope);
	item->setOpen(TRUE);
	scope = env->scopeObjectAt(level++);
    }
}

void QSVariablesContainer::updateMembers(QListViewItem *parent, QSObject *object)
{
    QSEnv *env = object->objectType()->env();
    QSMemberMap *mmap = object->objectType()->definedMembers();
    QSMemberMap::Iterator it = mmap->begin();
    for (; it != mmap->end(); ++it) {
	QSMember member = *it;
	if (member.type() == QSMember::Variable) {
	    QSObject value = object->fetchValue(member);
	    if (!(value.isA(env->typeClass()) ||
		  member.name() == "undefined" ||
		  member.name() == "NaN" ||
		  member.name() == "Infinity")) {
		QListViewItem *item  = new QListViewItem(parent,
							 member.name(),
							 value.toString(),
							 value.objectType()->name());
		if (value.objectType()->asClass()) {
		    updateMembers(item, &value);
		    item->setOpen(TRUE);
		}
	    }
	}
    }
}
