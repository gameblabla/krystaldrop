#ifndef Event_H
#define Event_H

class KD_EventManager;

/**
	An event. Once activated, it is called at each frame by the EventManager.
  */
class KD_Event
{
	friend class KD_EventManager;
protected:
	/**
		If true, Update and Display will be called on each frame.
		Else the event is left untouched.
	*/
	bool activate;

	/**
		If true, the event will be destructed after next Update.
	*/
	bool autoDestruct;

	/**
		Time elapsed since the event has been activated.
		Time is expressed in seconds.
	*/
	float timeElapsed;

	/**
		If timeElapsed is greater than countDown, then the Event will autodestruct itself.
		If countDown<0, countDown is disabled.
	*/
	float countDown;

	/**
		Update the event if it is activated.
		It basically calls Update(timeElapsed);
	*/
	void UpdateEvent(float timeElapsedSinceLastFrame);

	/**
		Displays the event if it is activated.
		It basically calls Display();
	*/
	void DisplayEvent();

public:
	KD_Event();
	virtual ~KD_Event();

	/**
		Update the event.
		This function should be overited by the daughter class.
	*/
	virtual void Update(float timeElapsed);

	/**
		Display the event.
		This function should be overited by the daughter class.
	*/
	virtual void Display();

	/**
		Set the autodestruction flag of the event
	*/
	void autoDestructEvent();

	/**
		Set the activate flag of the event
	*/
	void activateEvent();

	/**
		Pauses the event (sets the activate flag to false).
	*/
	void pauseEvent();

	/**
		Reset the timer of the event
	*/
	void resetTimer();

	/**
		Sets the time after which the event will autodestruct itself.
	*/
	void setCountDownTimer(float countDown);
};	

#endif
