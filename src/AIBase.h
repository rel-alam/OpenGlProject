#pragma once

#include <vector>
#include <stdlib.h>

// base class to define a zero-sum game with two opponents
// actions are defined as int's
class Game {
public:

	enum State {
		UNKNOWN,
		PLAYER_ONE,
		PLAYER_TWO,
		DRAW,
	};

	Game() :
		m_currentPlayer(PLAYER_ONE),
		m_currentOpponent(PLAYER_TWO) {

	}
	virtual ~Game() {}

	// returns wether someone is winning or not
	virtual State	getCurrentGameState() const = 0;

	// these switch whenever an action is made
	State			getCurrentPlayer() const	{ return m_currentPlayer;	}
	State			getCurrentOpponent() const	{ return m_currentOpponent; }

	// get a list of valid actions for current player
	virtual void	getValidActions(std::vector<int>& actions) const = 0;
	virtual bool	isActionValid(int action) const = 0;

	// performs an action for the current player, and switches current player
	virtual void	performAction(int action) = 0;

	// draw the game
	virtual void	draw() const = 0;

	// clones the current game state
	virtual Game*	clone() const = 0;

protected:

	State	m_currentPlayer;
	State	m_currentOpponent;
};

// base class for an A.I. opponent
class AI {
public:

	AI() {}
	virtual ~AI() {}

	// makes a decision for the current game
	virtual int	makeDecision(const Game& game) = 0;
};

// an A.I. opponent that chooses purely random actions to perform
class RandomAI : public AI {
public:

	RandomAI() {}
	virtual ~RandomAI() {}

	// randomly choose an action to perform
	virtual int	makeDecision(const Game& game) {
		std::vector<int> actions;
		game.getValidActions(actions);
		if (actions.size() > 0)
			return actions[rand() % actions.size()];
		else
			return -1;
	}
};

class MCTS : public AI
{
public:
	MCTS(int playouts) : m_playouts(playouts)
	{
	}
	virtual ~MCTS() {}

	virtual int makeDecision(const Game& game)
	{
		std::vector<int> actions;
		game.getValidActions(actions);
		int startValue = 0;

		int bestScore = 0;
		int bestAction = 0;

		std::vector<int> scores;


		Game* clone;
			for (int i = 0; i < actions.size(); ++i)
			{
				startValue = 0;
				for (int j = 0; j < m_playouts; ++j)
				{
					
					clone = game.clone();
					clone->performAction(i);
					while (clone->getCurrentGameState() == Game::UNKNOWN)
					{
						clone->performAction(rand() % actions.size());
					}
					if (clone->getCurrentGameState() == Game::PLAYER_TWO)
					{
						startValue++;
					}
					else if (clone->getCurrentGameState() == Game::PLAYER_ONE)
					{
						startValue--;
					}
					
					delete clone;
				}
			
				scores.push_back((float)m_playouts / (float)startValue);
			}

			for (int i = 0; i < scores.size(); ++i)
			{
				if (scores[i] > bestScore)
				{
					bestScore = scores[i];
					bestAction = i;
				}
					
			}
			return bestAction;
	}

private:

	int m_playouts;
};