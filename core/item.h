struct Item 
{
    int weight;
    int value;
    Item(int w, int v) : weight(w), value(v) {}
    Item() : weight(), value() {}
};