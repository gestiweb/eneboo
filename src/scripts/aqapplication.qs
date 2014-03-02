var form = this;
/***************************************************************************
                             aqapplication.qs
                            -------------------
   begin                : mie may 11 20011
   copyright            : (C) 2003-2011 by InfoSiAL S.L.
   email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************************************
 * Este programa es software libre. Puede redistribuirlo y/o modificarlo bajo los términos de la
 * Licencia Pública General de GNU en su versión 2, publicada por la Free Software Foundation.
 **************************************************************************************************/

const mainWindow_ = new MainWindow(this);

class DockListView
{
  var w_;
  var lw_;
  var ag_;

  function DockListView(mainWindow, name, title)
  {
    if (mainWindow == undefined)
      return;

    var w = this.w_ = new QDockWindow(AQS.InDock, mainWindow.w_, name);

    this.lw_ = new QListView(w, name + "ListView");
    this.lw_.addColumn("");
    this.lw_.addColumn("");
    this.lw_.setSorting(-1);
    this.lw_.rootIsDecorated = true;
    this.lw_.setColumnWidthMode(1, 0);
    this.lw_.hideColumn(1);
    this.lw_.header().hide();
    this.lw_.header().setResizeEnabled(false, 1);

    w.setWidget(this.lw_);
    w.setCaption(title);
    w.resizeEnabled = true;
    w.closeMode = true;
    w.setFixedExtentWidth(300);

    connect(this.lw_, "doubleClicked(QListViewItem*)", this, "activateAction()");
  }

  function writeState()
  {
    var w = this.w_;

    var settings = new AQSettings;
    var key = "MainWindow/" + w.name + "/";

    settings.writeEntry(key + "place", w.place());
    settings.writeEntry(key + "visible", w.visible);
    settings.writeEntry(key + "x", w.x);
    settings.writeEntry(key + "y", w.y);
    settings.writeEntry(key + "width", w.width);
    settings.writeEntry(key + "height", w.height);
    settings.writeEntry(key + "offset", w.offset);
    var area = w.area();
    settings.writeEntry(key + "index", area ? area.findDockWindow(w) : -1);
  }

  function readState()
  {
    var w = this.w_;

    var settings = new AQSettings;
    var key = "MainWindow/" + w.name + "/";

    var place = settings.readNumEntry(key + "place", AQS.InDock);
    if (place == AQS.OutsideDock) {
      w.undock();
      w.move(settings.readNumEntry(key + "x", w.x),
             settings.readNumEntry(key + "y", w.y));
    }
    w.offset = settings.readNumEntry(key + "offset", w.offset);
    var index = settings.readNumEntry(key + "index", -1);
    if (index != -1) {
      var area = w.area();
      if (area)
        area.moveDockWindow(w, index);
    }
    var width = settings.readNumEntry(key + "width", w.width);
    var height = settings.readNumEntry(key + "height", w.height);
    this.lw_.resize(width, height);
    w.resize(width, height);
    var visible = settings.readBoolEntry(key + "visible", true);
    if (visible)
      w.show();
    else
      w.hide();
  }

  function initFromWidget(w)
  {
    this.w_ = w;
    this.lw_ = w.widget();
    connect(this.lw_, "doubleClicked(QListViewItem*)", this, "activateAction()");
  }

  function activateAction(item)
  {
    if (item == undefined)
      return;
    var actionName = item.text(1);
    if (actionName == "")
      return;
    var ac = this.ag_.child(actionName, "QAction");
    if (ac != undefined)
      ac.activate();
  }

  function update(actionGroup, reverse)
  {
    this.ag_ = actionGroup;
    this.lw_.clear();
    if (actionGroup == undefined)
      return;
    this.buildListView(this.lw_, AQS.toXml(actionGroup),
                       actionGroup, reverse);
  }

  function buildListView(parentItem, parentElement, ag, reverse)
  {
    var thisItem = undefined;
    var node = reverse
               ? parentElement.lastChild().toElement()
               : parentElement.firstChild().toElement();

    while (!node.isNull()) {
      var className = node.attribute("class");
      if (node.tagName() == "object" && className.startsWith("Action")) {
        if (node.attribute("visible") == "false") {
          node = reverse
                 ? node.previousSibling().toElement()
                 : node.nextSibling().toElement();
          continue;
        }

        if (node.attribute("usesDropDown") == "false") {
          this.buildListView(parentItem, node, ag, reverse);
          node = reverse
                 ? node.previousSibling().toElement()
                 : node.nextSibling().toElement();
          continue;
        }

        if (thisItem == undefined)
          thisItem = new QListViewItem(parentItem);
        else
          thisItem = new QListViewItem(parentItem, thisItem);

        var actionName = node.attribute("name");
        var ac = ag.child(actionName);
        if (ac != undefined) {
          thisItem.setPixmap(0, new QPixmap(ac.iconSet().pixmap()));
          if (className == "Action")
            thisItem.setText(1, actionName);
        }
        thisItem.setText(0, node.attribute("menuText").replace("&", ""));
        if (node.attribute("enabled") == "false")
          thisItem.setEnabled(false);
        this.buildListView(thisItem, node, ag, reverse);
      }
      node = reverse
             ? node.previousSibling().toElement()
             : node.nextSibling().toElement();
    }
  }
}

class MainWindow
{
  const MAX_RECENT = 10;

  var w_;
  var app_;
  var agMenu_;
  var agRec_;
  var agMar_;
  var dckMod_;
  var dckRec_;
  var dckMar_;
  var tw_;
  var actSigMap_;
  var initializedMods_;
  var mainWidgets_;
  var ListaTabs_ = [];

  function MainWindow(app)
  {
    this.app_ = app;
  }

  function eventFilter(o, e)
  {
    switch (e.type) {
      case AQS.ContextMenu: {
        if (o.isEqual(this.dckMod_.w_)) {
          return this.addMarkFromItem(this.dckMod_.lw_.currentItem(),
                                      e.eventData.globalPos);
        }
        if (o.isEqual(this.dckRec_.w_)) {
          return this.addMarkFromItem(this.dckRec_.lw_.currentItem(),
                                      e.eventData.globalPos);
        }
        if (o.isEqual(this.dckMar_.w_)) {
          return this.removeMarkFromItem(this.dckMar_.lw_.currentItem(),
                                         e.eventData.globalPos);
        }
        var abanqMenu = this.w_.child("abanqMenu");
        abanqMenu.exec(e.eventData.globalPos);
        return true;
      }

      case AQS.Close: {
        if (aqApp.mainWidget() != undefined && 
            o.isEqual(aqApp.mainWidget()))
          return !this.exit();
      }
    }

    return false;
  }

  function createUi(uiFile)
  {
    var mng = aqApp.db().managerModules();
    this.w_ = mng.createUI(uiFile);
    this.w_.name = "container";
  }

  function exit()
  {
    var res = MessageBox.information(
                sys.translate("¿Quiere salir de la aplicación?"),
                MessageBox.Yes, MessageBox.No,
              MessageBox.NoButton, "Eneboo"
              );
    this.writeState();
    return MessageBox.Yes == res;
  }

  function writeState()
  {
    var w = this.w_;

    this.dckMod_.writeState();
    this.dckRec_.writeState();
    this.dckMar_.writeState();

    var settings = new AQSettings;
    var key = "MainWindow/";

    settings.writeEntry(key + "maximized", w.maximized);
    settings.writeEntry(key + "x", w.x);
    settings.writeEntry(key + "y", w.y);
    settings.writeEntry(key + "width", w.width);
    settings.writeEntry(key + "height", w.height);

    key += aqApp.db().database() + "/";

    var tw = this.tw_;
    var openActions = [];
    for (var i = 0; i < tw.count; ++i)
      openActions.push(tw.page(i).idMDI());
    settings.writeEntryList(key + "openActions", openActions);
    settings.writeEntry(key + "currentPageIndex", tw.currentPageIndex());

    var recentActions = [];
    var item = this.dckRec_.lw_.firstChild();
    while (item) {
      recentActions.push(item.text(1));
      item = item.nextSibling();
    }
    settings.writeEntryList(key + "recentActions", recentActions);

    var markActions = [];
    var item = this.dckMar_.lw_.firstChild();
    while (item) {
      markActions.push(item.text(1));
      item = item.nextSibling();
    }
    settings.writeEntryList(key + "markActions", markActions);
  }

  function readState()
  {
    var w = this.w_;
    var action;
    var moduleName;
    this.dckMod_.readState();
    this.dckRec_.readState();
    this.dckMar_.readState();

    var settings = new AQSettings;
    var key = "MainWindow/";

    var maximized = settings.readBoolEntry(key + "maximized");
    if (!maximized) {
      var x = settings.readNumEntry(key + "x");
      var y = settings.readNumEntry(key + "y");
      if (sys.osName() == "MACX" && y < 20)
        y = 20;
      w.move(x, y);
      w.resize(settings.readNumEntry(key + "width", w.width),
               settings.readNumEntry(key + "height", w.height));
    } else
      w.showMaximized();

    if (this.agMenu_ != undefined) {
      key += aqApp.db().database() + "/";

      var tw = this.tw_;
      var openActions = settings.readListEntry(key + "openActions");
      for (var i = 0; i < tw.count; ++i)
        tw.page(i).close();
      for (var i = 0; i < openActions.length; ++i)
                {
                 action = this.agMenu_.child(openActions[i], "QAction");
				 moduleName = aqApp.db().managerModules().idModuleOfFile(action.name + ".ui");
                 if (moduleName != undefined && moduleName != "")
                	{
                	this.initModule(moduleName);
               		this.addForm(openActions[i],action.iconSet().pixmap());
               		}
               	else
               		{
               		print ("aqAppScript : No se ha podido inicializar el módulo de la \"Action\" " + action.name + ".La pestaña es omitida.");
               		}
                 }
      var idx = settings.readNumEntry(key + "currentPageIndex");
      if (idx >= 0 && idx < tw.count)
        tw.setCurrentPage(idx);

      var recentActions = settings.readListEntry(key + "recentActions");
      for (var i = recentActions.length - 1; i >= 0; --i)
        this.addRecent(this.agMenu_.child(recentActions[i], "QAction"));

      var markActions = settings.readListEntry(key + "markActions");
      for (var i = 0; i < markActions.length; ++i)
        this.addMark(this.agMenu_.child(markActions[i], "QAction"));
    }
  }

  function init()
  {
    var w = this.w_;

    w.statusBar().hide();
    this.mainWidgets_ = [];
    this.initializedMods_ = [];
    this.actSigMap_ = new QSignalMapper(this.w_, "abanqActSigMap");
    connect(this.actSigMap_, "mapped(const QString&)",
            this.app_, "triggerAction()");
    this.initTabWidget();
    this.initHelpMenu();
    this.initConfigMenu();
    this.initTextLabels();
    this.initDocks();
    this.initEventFilter();
  }

  function initFromWidget(w)
  {
    this.w_ = w;
    this.mainWidgets_ = [];
    this.initializedMods_ = [];
    this.actSigMap_ = new QSignalMapper(this.w_, "abanqActSigMap");
    connect(this.actSigMap_, "mapped(const QString&)",
            this.app_, "triggerAction()");
    this.tw_ = w.child("tabWidget", "QTabWidget");
    this.agMenu_ = w.child("abanqActionGroup", "QActionGroup");
    this.dckMod_ = new DockListView;
    this.dckMod_.initFromWidget(w.child("abanqDockModules", "QDockWindow"));
    this.dckRec_ = new DockListView;
    this.dckRec_.initFromWidget(w.child("abanqDockRecent", "QDockWindow"));
    this.dckMar_ = new DockListView;
    this.dckMar_.initFromWidget(w.child("abanqDockMarks", "QDockWindow"));
    this.initEventFilter();
  }

  function initEventFilter()
  {
    var w = this.w_;

    w.eventFilterFunction = "aqAppScript.mainWindow_.eventFilter";
    w.allowedEvents = [ AQS.ContextMenu, AQS.Close ];
    w.installEventFilter(w);

    this.dckMod_.w_.installEventFilter(w);
    this.dckRec_.w_.installEventFilter(w);
    this.dckMar_.w_.installEventFilter(w);
  }

  function initModule(module)
  {
    if (module in this.mainWidgets_) {
      var mwi = this.mainWidgets_[module];
      mwi.name = module;
      aqApp.name = module;
      mwi.show();
    }
    if (!(module in this.initializedMods_) || 
        this.initializedMods_[module] != true) {
      this.initializedMods_[module] = true;
      aqApp.call("init", module);
    }
    var mng = aqApp.db().managerModules();
    mng.setActiveIdModule(module);
  }

  function addForm(actionName,Pixmap)
  {
    var tw = this.tw_;
    var fm = new AQFormDB(actionName, tw, 0);

    fm.setMainWidget();
    if (fm.mainWidget() == undefined)
      return;

    var idx = undefined;
    var texto = undefined;
    var visible = false; 
    if ((actionName in this.ListaTabs_)) texto = ListaTabs_[actionName]; 
    
    if(texto != undefined) //Si existe el texto de ese actionName
                     { 
                      texto = this.ListaTabs_[actionName];
                      
                    for (var i = tw.count; i >= 0 ; i--) {
                   if (tw.tabLabel(tw.page(i)) == texto)  //Si la pestaña es visible...
                                                {
                                                tw.page(i).close();
                                                /*this.ListaTabs_[actionName] = texto;
                                                // TODO: ELIMINAR ESTA PESTaÑA
                                                visible = true;
                                                idx = i;*/
                                                
                                                }
                                       }
                        
                    }
   if (!visible) 
         {
	texto = fm.caption //Cogemos el nuevo título

         tw.addTab(fm,new QIconSet(iconSet16x16(new QPixmap(Pixmap))), texto);
         idx = tw.indexOf(fm);
         fm.setIdMDI(actionName);
         fm.show();
         this.ListaTabs_[actionName]=texto;
         } 
   tw.setCurrentPage(idx);  // llamamos a nuestra pestaña
}

  function addRecent(action)
  {
    if (action == undefined)
      return;
    if (this.agRec_ == undefined)
      this.agRec_ = new QActionGroup(this.w_, "abanqAgRec");

    var agr = this.agRec_;
    var ac = agr.child(action.name, "QAction");
    var checkMax = true;
    if (ac != undefined) {
      agr.removeChild(ac);
      checkMax = false;
    }
    ac = this.cloneAction(action, agr);
    connect(ac, "activated()", action, "activate()");

    var lw = this.dckRec_.lw_;
    if (checkMax && lw.childCount >= this.MAX_RECENT) {
      ac = agr.child(lw.lastItem().text(1), "QAction");
      if (ac != undefined)
        agr.removeChild(ac);
    }
    this.dckRec_.update(agr, true);
  }

  function addMark(action)
  {
    if (action == undefined)
      return;
    if (this.agMar_ == undefined)
      this.agMar_ = new QActionGroup(this.w_, "abanqAgMar");

    var ac = this.cloneAction(action, this.agMar_);
    connect(ac, "activated()", action, "activate()");

    this.dckMar_.update(this.agMar_);
  }

  function addMarkFromItem(item, pos)
  {
    if (item == undefined)
      return false;
    if (item.text(1).isEmpty())
      return true;

    var popMenu = new QPopupMenu;
    popMenu.insertItem(sys.translate("Añadir a Marcadores"), 1);
    if (popMenu.exec(pos) == 1) {
      var ac = this.agMenu_.child(item.text(1));
      if (ac != undefined)
        this.addMark(ac);
    }
    return true;
  }

  function removeMarkFromItem(item, pos)
  {
    if (item == undefined || this.agMar_ == undefined || 
        this.dckMar_.lw_.childCount == 0)
      return false;
    if (item.text(1).isEmpty())
      return true;

    var popMenu = new QPopupMenu;
    popMenu.insertItem(sys.translate("Eliminar Marcador"), 1);
    if (popMenu.exec(pos) == 1) {
      var ac = this.agMar_.child(item.text(1));
      if (ac != undefined) {
        this.agMar_.removeChild(ac);
        this.dckMar_.update(this.agMar_);
      }
    }
    return true;
  }

  function updateMenuAndDocks()
  {
    var w = this.w_;
    
    this.updateActionGroup();
    var abanqMenu = w.child("abanqMenu");
    abanqMenu.clear();
    this.agMenu_.addTo(abanqMenu);
  
    aqApp.setMainWidget(w);

    this.dckMod_.update(this.agMenu_);
    this.dckRec_.update(this.agRec_);
    this.dckMar_.update(this.agMar_);

    connect(w.child("aboutQtAction"), "activated()", aqApp, "aboutQt()");
    connect(w.child("aboutAbanQAction"), "activated()", aqApp, "aboutAbanQ()");
    connect(w.child("fontAction"), "activated()", aqApp, "chooseFont()");
    connect(w.child("helpIndexAction"), "activated()", aqApp, "helpIndex()");
    connect(w.child("urlEnebooAction"), "activated()", aqApp, "urlEneboo()");
  }

  function updateActionGroup()
  {
    var w = this.w_;

    if (this.agMenu_ != undefined)
    {
      var list = new AQObjectQueryList(this.agMenu_, "QAction", "",
                                       true, true);
      var obj;
      while ((obj = list.current()) != undefined) {
        this.agMenu_.removeChild(obj);
        list.next();
      }
      w.removeChild(this.agMenu_);
      this.agMenu_ = undefined;
    }

    var agm = new QActionGroup(w, "abanqActionGroup");
    this.agMenu_ = agm;
    agm.menuText = sys.translate("Menú");

    var mng = aqApp.db().managerModules();
    var areas = mng.listIdAreas();

    for (var i = 0; i < areas.length; ++i) {
      var ag = new QActionGroup(agm);
      ag.name = areas[i];
      if (!sys.isDebuggerEnabled() && ag.name == "sys") break;
      ag.menuText = ag.text = mng.idAreaToDescription(ag.name);
      ag.usesDropDown = true;
      ag.setIconSet(new QIconSet(AQS.Pixmap_fromMimeSource("folder.png")));

      var modules = mng.listIdModules(ag.name);
      for (var j = 0; j < modules.length; ++j) {
        var ac = new QActionGroup(ag);
        ac.name = modules[j];
        if (sys.isQuickBuild()) {
            if (ac.name == "sys") {
                continue;
            }
        }

        var actions = this.widgetActions(ac.name + ".ui", ac);
        if (actions == undefined) {
          ac.name = "unnamed";
          ac.deleteLater();
          ac = new QAction(ag);
          ac.name = modules[j];
        }

        ac.menuText = ac.text = mng.idModuleToDescription(ac.name);
        ac.usesDropDown = true;
        ac.setIconSet(this.iconSet16x16(mng.iconModule(ac.name)));

        connect(ac, "activated()", this.actSigMap_, "map()");
        this.actSigMap_.setMapping(ac, "activated():initModule():" + ac.name);
        if (ac.name == "sys" && ag.name == "sys")
          {
            if (sys.isDebuggerMode())
              {
                var wb = new QAction(ag);
                wb.name = "runWorkBench";
                wb.menuText = sys.translate("QSA Work Bench");
                wb.setIconSet(new QIconSet(this.iconSet16x16(AQS.Pixmap_fromMimeSource("bug.png"))));
                connect(wb, "activated()", this.actSigMap_, "map()");
		this.actSigMap_.setMapping(wb, "activated():runWorkBench():" + wb.name);
		
		var sLoad = new QAction(ag);
    		sLoad.name = "StaticLoadAction";
    		sLoad.menuText = sys.translate("Cargar &Estática desde Disco Duro");
    		sLoad.setIconSet(new QIconSet(AQS.Pixmap_fromMimeSource("image-svg.png")));
    		connect(sLoad, "activated()", this.actSigMap_, "map()");
    		this.actSigMap_.setMapping(sLoad, "activated():staticLoad():" + sLoad.name);

    		var resetSC = new QAction(ag);
    		resetSC.name = "resetScriptAction";
    		resetSC.menuText = sys.translate("Resetear &Scripts");
    		resetSC.setIconSet(new QIconSet(AQS.Pixmap_fromMimeSource("reload.png")));
    		connect(resetSC, "activated()", this.actSigMap_, "map()");
    		this.actSigMap_.setMapping(resetSC, "activated():resetSC():" + resetSC.name);

              }
           }
      }
    }

    agm.addSeparator();

    var exit = new QAction(agm);
    exit.name = "exitAction";
    exit.menuText = sys.translate("&Salir");
    exit.setIconSet(new QIconSet(AQS.Pixmap_fromMimeSource("exit.png")));
    connect(exit, "activated()", this.actSigMap_, "map()");
    this.actSigMap_.setMapping(exit, "activated():exit():" + exit.name);

  }

  function initTabWidget()
  {
    var w = this.w_;

    var tw = this.tw_ = w.child("tabWidget", "QTabWidget");
    tw.removePage(tw.page(0));
  }

  function initHelpMenu()
  {
    var w = this.w_;

    var aboutQt = w.child("aboutQtAction");
    aboutQt.setIconSet(
      this.iconSet16x16(AQS.Pixmap_fromMimeSource("aboutqt.png"))
    );
    connect(aboutQt, "activated()", aqApp, "aboutQt()");

    var aboutAbanQ = w.child("aboutAbanQAction");
    aboutAbanQ.setIconSet(
      this.iconSet16x16(AQS.Pixmap_fromMimeSource("icono_abanq.png"))
    );
    connect(aboutAbanQ, "activated()", aqApp, "aboutAbanQ()");

   var helpIndex = w.child("helpIndexAction");
    helpIndex.setIconSet(
      this.iconSet16x16(AQS.Pixmap_fromMimeSource("help_index.png"))
    );
    connect(helpIndex, "activated()", aqApp, "helpIndex()");
    
    var urlEneboo = w.child("urlEnebooAction");
    urlEneboo.setIconSet(
      this.iconSet16x16(AQS.Pixmap_fromMimeSource("icono_abanq.png"))
    );
    connect(urlEneboo, "activated()", aqApp, "urlEneboo()");
  }

  function initConfigMenu()
  {
    var w = this.w_;

    var font = w.child("fontAction");
    font.setIconSet(new QIconSet(AQS.Pixmap_fromMimeSource("font.png")));
    connect(font, "activated()", aqApp, "chooseFont()");
  }

    function initTextLabels()
    {
    var w = this.w_;
    var tL = w.child("tLabel");
    var tL2 = w.child("tLabel2");
    var texto = AQUtil.sqlSelect("flsettings", "valor", "flkey='verticalName'");
    if (texto)
    tL.text = texto;
    tL2.text = sys.nameUser()+"@"+sys.nameBD();
    if (sys.osName() == "MACX") 
           tL2.text +="     ";
    
    }

  function initDocks()
  {
    var w = this.w_;

    this.dckMar_ = new DockListView(this, "abanqDockMarks",
                                    sys.translate("Marcadores"));
    w.moveDockWindow(this.dckMar_.w_, AQS.DockLeft);

    this.dckRec_ = new DockListView(this, "abanqDockRecent",
                                    sys.translate("Recientes"));
    w.moveDockWindow(this.dckRec_.w_, AQS.DockLeft);

    this.dckMod_ = new DockListView(this, "abanqDockModules",
                                    sys.translate("Módulos"));
    w.moveDockWindow(this.dckMod_.w_, AQS.DockLeft);

    var windowMenu = w.child("windowMenu");
    windowMenu.insertItem(sys.translate("&Vistas"),
                          w.createDockWindowMenu(AQS.NoToolBars));
  }

  function cloneAction(act, parent)
  {
    var ac = new QAction(parent);
    ac.name = act.name;
    ac.text = act.text;
    ac.menuText = act.menuText;
    ac.statusTip = act.statusTip;
    ac.toolTip = act.toolTip;
    ac.whatsThis = act.whatsThis;
    ac.enabled = act.enabled;
    ac.visible = act.visible;
    ac.on = act.on;
    ac.toggleAction = act.toggleAction;
    ac.setIconSet(this.iconSet16x16(act.iconSet().pixmap()));
    return ac;
  }

  function addActions(node, actGroup, wi)
  {
    var actions = node.elementsByTagName("action");
    for (var i = 0; i < actions.length(); ++i) {
      var itn = actions.item(i).toElement();
      var acw = wi.child(itn.attribute("name"), "QAction");
      if (acw == undefined)
        continue;
      var prev = itn.previousSibling().toElement();
      if (!prev.isNull() && prev.tagName() == "separator")
        actGroup.addSeparator();
      this.cloneAction(acw, actGroup);
    }
  }

  function widgetActions(uiFile, parent)
  {
    var mng = aqApp.db().managerModules();
    var w = mng.createUI(uiFile);

    if (w == undefined || !w.inherits("QMainWindow")) {
      if (w != undefined)
        this.mainWidgets_[w.name] = w;
      return undefined;
    }

    w.name = parent.name;
    aqApp.setMainWidget(w);
      
    var doc = new QDomDocument;
    if (!doc.setContent(mng.contentCached(uiFile)))
      return undefined;
    var root = doc.documentElement().toElement();

    var ag = new QActionGroup(parent);
    ag.name = parent.name + "Actions";
    ag.menuText = ag.text = sys.translate("Acciones");

    var bars = root.namedItem("toolbars").toElement();
    addActions(bars, ag, w);

    var menu = root.namedItem("menubar").toElement();
    var items = menu.elementsByTagName("item");
    if (items.length() > 0) {
      ag.addSeparator();
      var menuAg = new QActionGroup(ag);
      menuAg.name = ag.name + "More";
      menuAg.menuText = menuAg.text = sys.translate("Más");
      menuAg.usesDropDown = true;
      menuAg.setIconSet(new QIconSet(AQS.Pixmap_fromMimeSource("plus.png")));

      for (var i = 0; i < items.length(); ++i) {
        var itn = items.item(i).toElement();
        var subMenuAg = new QActionGroup(menuAg);
        subMenuAg.menuText = sys.toUnicode(itn.attribute("text"), "UTF-8");
        subMenuAg.usesDropDown = true;
        addActions(itn, subMenuAg, w);
      }
    }

    var conns = root.namedItem("connections").toElement();
    var connections = conns.elementsByTagName("connection");
    for (var i = 0; i < connections.length(); ++i) {
      var itn = connections.item(i).toElement();
      var sender = itn.namedItem("sender").toElement().text();
      var ac = ag.child(sender);
      if (ac != undefined) {
        var signal = itn.namedItem("signal").toElement().text();
        var slot = itn.namedItem("slot").toElement().text();
        connect(ac, signal, this.actSigMap_, "map()");
        this.actSigMap_.setMapping(ac, signal + ":" + slot + ":" + ac.name);
      }
    }

    return ag;
  }

  function iconSet16x16(pix)
  {
    var pix = new QPixmap(pix);
    var img = pix.convertToImage();
    img = img.smoothScale(16, 16);
    return new QIconSet(new QPixmap(img));
  }

  function show()
  {
    this.w_.show();
  }

  function close()
  {
    this.w_.close();
  }
}

function initScript()
{
  var mw = this.mainWindow_;
  mw.createUi("mainwindow.ui");
  mw.init();
  mw.updateMenuAndDocks();
  mw.initModule("sys");
  mw.show();
  mw.readState();
}

function reinitScript()
{
  var mainWid = (this.mainWindow_.w_ == undefined)
                ? aqApp.mainWidget()
                : this.mainWindow_.w_;
  if (mainWid == undefined || 
      mainWid.name != "container" || 
      mainWid.isEqual(this.mainWindow_.w_))
    return;
  var mw = this.mainWindow_;
  mw.initFromWidget(mainWid);
  mw.writeState();
  mw.updateMenuAndDocks();
  mw.initModule("sys");
  mw.readState();
}

function aqAppScriptMain()
{
}

function triggerAction(signature)
{
  var mw = this.mainWindow_;
  var sgt = signature.split(":");
  var ok = true;

  var ac = mw.agMenu_.child(sgt[2]);
  if (ac == undefined) {
    ok = false;
    debug("triggerAction: Action not found: " + signature);
  }

  switch (sgt[0]) {
    case "activated()":
      if (!ac.visible || !ac.enabled)
        ok = false;
      break;

    default:
      ok = false;
      debug("triggerAction: Unhandled signal: " + signature);
      break;
  }

  switch (sgt[1]) {
    case "initModule()":
      if (ok)
        mw.initModule(ac.name);
      break;

    case "openDefaultForm()":
      if (ok) {
        mw.addForm(ac.name,ac.iconSet().pixmap());
        mw.addRecent(ac);
      }
      break;

    case "execDefaultScript()":
      if (ok) {
        aqApp.execMainScript(ac.name);
        mw.addRecent(ac);
      }
      break;

    case "loadModules()":
      if (ok)
        sys.loadModules();
      break;

    case "exportModules()":
      if (ok)
        sys.exportModules();
      break;

    case "importModules()":
      if (ok)
        sys.importModules();
      break;

    case "updateAbanQ()":
      if (ok)
        sys.updateAbanQ();
      break;

    case "runWorkBench()":
      if (ok)
        sys.openQSWorkbench();
      break;
    
    case "staticLoad()":
	if (ok) 
        aqApp.staticLoaderSetup();
      break;
    
    case "resetSC()":
      if (ok)
        aqApp.reinit();
      break;

    case "dumpDatabase()":
      if (ok)
        sys.dumpDatabase();
      break;

    case "exit()":
      if (ok)
        mw.close();
      break;

    default:
      ok = false;
      debug("triggerAction: Unhandled slot: " + signature);
      break;
  }
}

