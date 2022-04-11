package SliderApp;

import java.util.Observable;
import java.util.ArrayList;
import java.util.List;

class ValueModel {

    int value = 2;
    List<ModelView> observers = new ArrayList<ModelView>();

    int getValue() { return value; }

    void setValue(int newValue) { 
	int oldValue = value;
	value = newValue; 
	for (ModelView view : observers) {
	    view.update(oldValue);
	}
    }

    void addObserver(ModelView view) { observers.add(view); }
}