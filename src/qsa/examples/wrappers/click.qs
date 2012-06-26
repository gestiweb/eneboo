connect(listView, "clicked(QListViewItem*)", clicked);
connect(listView, "returnPressed(QListViewItem*)", clicked);
connect(listView, "spacePressed(QListViewItem*)", clicked);
var clickCount = 0;

function clicked(item)
{
    item.text = "Item clicked %1".arg(clickCount++);
}
