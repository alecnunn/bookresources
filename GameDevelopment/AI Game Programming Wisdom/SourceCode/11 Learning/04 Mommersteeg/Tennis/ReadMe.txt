TENNIS: SEQUENTIAL PREDICTION DEMO
by Fri Mommersteeg (fri.mommersteeg@home.nl)

This tennis game demonstrates the positioning pattern, as discussed in my article 'Pattern Recognition with Sequential Prediction'. The demo is set up to show the pattern recognition capabilities of sequential prediction. 

The tennis bot uses four different positioning strategies:
1. interpolate the motion of the ball to where it will hit the ground; move to that position,
2. move directly toward the ball,
3. move to the center of the play field,
4. move toward a position where the ball is expected to go to.

The first two strategies are used when the ball is heading towards the bot's play field. The latter two strategies are used when the ball 
is heading towards the gamer's play field. Strategy 4 uses sequential prediction. For this purpose, the bot is equipped with two binary 
predictors. The first predictor decides whether the ball will hit the ground above or below the center. The second predictor decides 
whether the ball will hit the ground to the left or to the right of the center. This allows the bot to choose its horizontal and vertical 
position independently. 

The bot uses a simple performance indicator to determine whether the predictions are good or bad. When a prediction is expected to be good, the bot uses the prediction (this is indicated by a red cross on the bot's play field). Otherwise the bot will ignore the prediction and return to the center of the play field. Depending on how well the prediction is expected to be, the bot will anticipate more or less.

Play with this demo to learn the strengths and weaknesses of sequential prediction. Here are some simple patterns that you could generate to see how the bot reacts:

- continually play the ball to the same spot of the bot's play field
- continually play the ball from the left to the right of the play field
- generate a pattern that's a bit more complex, like (left, left, right) repeated.

To make it easier for you to generate patterns, the AI will always return the ball in a straight line. However, if you think you're ready for the real game, hit F2 at any time to enable bot targeting. Good luck! :-)


CONTROLS:

Arrow Keys              move paddle
Z, X                    turn paddle 
ESC                     quit demo
