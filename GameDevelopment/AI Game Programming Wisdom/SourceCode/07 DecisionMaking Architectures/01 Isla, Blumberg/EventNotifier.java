package bb;

/*
A so-far fairly empty event interface. Right now all it does is
distribute information about agent deaths to whoever is interested
(i.e. whatever class IMPLEMENTS the EventNotifier interface and
registers with the world to receive the event info).
*/

public interface EventNotifier {
    public void deathNotification(Agent a, Vec2 location);
}