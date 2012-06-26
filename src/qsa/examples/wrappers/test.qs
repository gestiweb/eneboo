connect(listView, "selectionChanged(QListViewItem*)", changed);

function changed(item)
{
    var color = new Color(item.text);
    var palette = listView.palette();
    var active = palette.active;
    active.base = color;
    active.highlight = color.dark();
    palette.active = active;
    listView.setPalette(palette);
}

