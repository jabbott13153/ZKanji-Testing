//---------------------------------------------------------------------------

#ifndef mouseeventH
#define mouseeventH
//---------------------------------------------------------------------------

// ----- MouseRepeat -----
// Call mouserepeat to simulate a pressed mouse button that needs to send repeated messages
// aevent - the mouse event that called this function
// Sender, Button, Shift, x, y - the variables sent to that event
// activearea - The mouse events are only repeated while the mouse cursor is in this area.
//              This area is always used for the next repeat.
//              When NULL it is always set to the area of the Sender.
// ----- MouseStopRepeat -----
// Stop sending the simulated button presses to the control.
void MouseRepeat(TMouseEvent aevent,TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y, TRect *activearea = NULL);
void MouseStopRepeat();
#endif
