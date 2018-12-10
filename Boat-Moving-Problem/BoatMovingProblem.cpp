// Author: Kermit Mitchell III
// Start Date: 09/18/2018 4:30 PM | Last Edited: 09/25/2018 9:45 AM
/* This program solves the Missionary and Cannibals problem, getting them to other side,
 Using AI */
// NOTE: The vector implmentation of frontier also works; for the queue implementation, uncomment all

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
using namespace std;

// Function Prototypes //

// This displays an error message when something invalid happens
void ErrorMessage(string msg);

// Structs and Classes //

// This describes a side on the riverbank
struct Side
{
	int m = 0; // the # of missionaries on this side
	int c = 0; // the # of cannibals on this side
	bool sideNum = 0; // 0 for original side, 1 for across side

public:

	void MoveMissionary(Side* s, int amount)
	{
		// Only 1 or 2 or 3 missionaries can be sent

		if (amount != 1 && amount != 2 && amount != 3)
		{
			ErrorMessage("Amount can ONLY be 1 or 2 or 3.");
		}
		else
		{
			// Move this amount of missonaries from this Side to the other Side s
			this->m -= amount;
			s->m += amount;
		}
	}

	void MoveCannibal(Side* s, int amount)
	{
		// Only 1 or 2 or 3 cannibals can be sent

		if (amount != 1 && amount != 2 && amount != 3)
		{
			ErrorMessage("Amount can ONLY be 1 or 2 or 3.");
		}
		else
		{
			// Move this amount of cannibals from this Side to the other Side s
			this->c -= amount;
			s->c += amount;
		}
	}

	void DisplaySide()
	{
		if (this->sideNum)
		{
			cout << "Across Side" << endl;
		}
		else
		{
			cout << "Original Side" << endl;
		}
		cout << "Missionaries: " << this->m << endl;
		cout << "Cannibals: " << this->c << endl;
	}
};

// This describes a state, a snapshot of the riverbank
struct State
{
	Side original;
	Side across;
	bool boatSide = 0; // 0 for orignal side, 1 for across side

public:

	void DisplayState()
	{
		original.DisplaySide();
		across.DisplaySide();
		if (this->boatSide)
		{
			cout << "Boat is on Across Side" << endl;
		}
		else
		{
			cout << "Boat is on Original Side" << endl;
		}
	}

	bool SafetyCheck()
	{
		// If there are more cannibals than missionaries on either side, then it's unsafe!
		if ((original.m != 0 && original.c > original.m) || (across.m != 0 && across.c > across.m))
		{
			cout << "This state is unsafe!" << endl;
			cout << "---------------------" << endl;
			return false;
		}
		else
		{
			cout << "This state is safe... for now." << endl;
			cout << "---------------------" << endl;
			return true;
		}
	}

	// Used to compare states
	bool operator==(const State& s)
	{
		bool result = false;

		if (this->original.m == s.original.m && this->original.c == s.original.c && 
			this->original.sideNum == s.original.sideNum && this->across.m == s.across.m && this->across.c == s.across.c &&
			this->across.sideNum == s.across.sideNum && this->boatSide == s.boatSide)
		{
			result = true;
		}

		return result;
	}
};

// This describes a node in a tree of states, that we will use BFS to go through
struct Node
{
	//Node* parent = nullptr; // this state's parent node
	State state; // this current State
	int optionNum = 0; // the option number 1-5 that explains how to get FROM the parent TO this current state 

	int q = -1; // which leaf in the tree you are
	int k = -1; // which leaf in the frontier you are

	int parentK = -1; // which k the this leaf stems from
	//int parentQ = -1; // which q the this leaf stems from
};

// This is the AI trying an option out of the 5 possible options
State AttemptDecision(State current, int optionNum);

// This is to see if the state has already been visted
bool Contains(vector<State>& v, State state);

// Used to find and retrieve the position of the node containing the goal, from the frontier
int GetGoalNodePosition(vector<Node> f, State theGoal);


int main()
{
	// Declare the variables

	// This is the starting state
	State starting;
	
	// This is the desired state
	State goal;
	
	// This is the tree
	vector<Node> tree;

	Node root;
	//root.parent = nullptr;
	
	// This is a vector of all visted states
	vector<State> visited;

	// This is the frontier
	vector<Node> frontier;
	//queue<Node> frontier;

	// This is the Stack for retracting steps
	stack<int> commands;

	bool boatRule = 0; // 0 for boat size 2, 1 for boat size 3
	int numOfPairs = 0; // number of pairs allowed;

	int numOfOptions;

	// Ask for user input

	cout << "Enter the problem version (0 for boatSize = 2 or 1 for boatSize = 3): " << endl;
	cin >> boatRule;

	cout << "Enter the amount of pairs (3 or 4 or 5): ";
	cin >> numOfPairs;

	// Setup the problem


		// Starting state
		starting.original.m = numOfPairs;
		starting.original.c = numOfPairs;
		starting.across.sideNum = 1;

		// Goal state
		goal.across.m = numOfPairs;
		goal.across.c = numOfPairs;
		goal.across.sideNum = 1;
		goal.boatSide = 1;

		// The number options is (2^boatSize) + 1
		if(boatRule)
		{
			numOfOptions = 9;
		}
		else
		{
			numOfOptions = 5;
		}
		
	// Root node
	root.state = starting;
	tree.push_back(root);

	// Frontier
	frontier.push_back(root);
	//frontier.push(root);

	// Show the first step
	cout << "STARTING STATE" << endl;
	starting.DisplayState();
	starting.SafetyCheck();

	// Process the data

	
	int i = 1; // an indexer for trying options 1-5 for version 0 or 1-9 for version 1
	int k = 0; // an indexer for keeping place in the frontier
	int q = 0; // and indexer for keeping place in the tree
	while (!(Contains(visited, goal))) // Keep looping until the goal has been visited
	{
		// Determine the position in the tree traversal
		Node current = frontier.at(k);

		//Node current = frontier.front();
		//frontier.pop();

		//Node* memAdr = &frontier.at(k);

		// Create the temp state for manipulation
		State temp;

		// Reset i
		i = 1;

		// Expand the frontier
		while (i <= numOfOptions)
		{
			// Try your options
			temp = AttemptDecision(current.state, i);
			temp.DisplayState();
			bool sc = temp.SafetyCheck();

			// Add current to visted, if it's not in there
			if (!Contains(visited, temp))
			{
				visited.push_back(temp);

				// Create a new node n, and add that to the tree
				q++;
				Node n;
				//n.parent = memAdr;
				n.state = temp;
				n.optionNum = i;

				n.q = q;
				n.parentK = k;
				//n.parentQ = current.q;

				tree.push_back(n);
				
				// If this is a new safe state, add to the frontier
				if (sc)
				{
					n.k = k;

					frontier.push_back(n);
					//frontier.push(n);

					if (temp == goal)
					{
						cout << "GOAL REACHED!" << endl;
						break;
					}
				}

			}

			// Increment i
			i++;

		}

		// Increment k to move to the next Node in the frontier
		k++;

		system("pause");
	
	}
	
	system("pause");
	system("CLS");

	// Display the results

 	cout << "HERE are the final steps: " << endl << endl;

	// Find the node containing the goal, and backtrace to a Stack
	Node goalNode = tree.back(); //tree.at(GetGoalNodePosition(tree, goal));


	commands.push(goalNode.optionNum);
	//Node* myPtr = goalNode.parent;
	int parentK = goalNode.parentK;
	//int parentQ = goalNode.parentQ;

	while (/*myPtr != nullptr*/parentK/*parentQ*/ /*!= -1*/ != 0)
	{
		/*
		commands.push(myPtr->optionNum);
		myPtr = myPtr->parent; */

		if (parentK/*parentQ*/ == 0)
		{
			break;
		}

		commands.push(frontier.at(parentK).optionNum);
		parentK = frontier.at(parentK).parentK;

		//commands.push(tree.at(parentQ).optionNum);
		//parentQ = tree.at(parentQ).parentQ;


	}

	// Create the temp state for manipulation
	State temp2;
	temp2 = starting;
	temp2.DisplayState();
	temp2.SafetyCheck();

	while (!commands.empty())
	{
		// Re-enact the decisions

		temp2 = AttemptDecision(temp2, commands.top());
		temp2.DisplayState();
		temp2.SafetyCheck();
		commands.pop();
	}

	cout << "GOAL REACHED!" << endl;

	// End the program
	system("pause");
	return 0;

}

void ErrorMessage(string msg)
{
	cout << "Error! " << msg << endl;
}

State AttemptDecision(State current, int optionNum)
{
	State newState = current;

	// Check the optionNum

	switch (optionNum)
	{
	default:
		cout << "Invalid option. Options range from 1-5." << endl;
		break;

	case 1:
		cout << "Option 1: Move 1 missionary over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.m < 1)
			{
				ErrorMessage("Insufficient missionaries: " + newState.across.m);
			}
			else
			{
				newState.across.MoveMissionary(&newState.original, 1);
				newState.boatSide = !newState.boatSide;
			}
		}
		else
		{
			if (newState.original.m < 1)
			{
				ErrorMessage("Insufficient missionaries: " + newState.original.m);
			}
			else
			{
				newState.original.MoveMissionary(&newState.across, 1);
				newState.boatSide = !newState.boatSide;
			}
		}
		break;

	case 2:
		cout << "Option 2: Move 2 missionaries over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.m < 2)
			{
				ErrorMessage("Insufficient missionaries: " + newState.across.m);
			}
			else
			{
				newState.across.MoveMissionary(&newState.original, 2);
				newState.boatSide = !newState.boatSide;
			}
		}
		else
		{
			if (newState.original.m < 2)
			{
				ErrorMessage("Insufficient missionaries: " + newState.original.m);
			}
			else
			{
				newState.original.MoveMissionary(&newState.across, 2);
				newState.boatSide = !newState.boatSide;
			}
		}
		break;

	case 3:
		cout << "Option 3: Move 1 cannibal over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.c < 1)
			{
				ErrorMessage("Insufficient cannibals: " + newState.across.c);
			}
			else
			{
				newState.across.MoveCannibal(&newState.original, 1);
				newState.boatSide = !newState.boatSide;
			}
		}
		else
		{
			if (newState.original.c < 1)
			{
				ErrorMessage("Insufficient cannibals: " + newState.original.c);
			}
			else
			{
				newState.original.MoveCannibal(&newState.across, 1);
				newState.boatSide = !newState.boatSide;
			}
		}
		break;

	case 4:
		cout << "Option 4: Move 2 cannibals over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.c < 2)
			{
				ErrorMessage("Insufficient cannibals: " + newState.across.c);
			}
			else
			{
				newState.across.MoveCannibal(&newState.original, 2);
				newState.boatSide = !newState.boatSide;
			}
		}
		else
		{
			if (newState.original.c < 2)
			{
				ErrorMessage("Insufficient cannibals: " + newState.original.c);
			}
			else
			{
				newState.original.MoveCannibal(&newState.across, 2);
				newState.boatSide = !newState.boatSide;
			}
		}
		break;

	case 5:
		cout << "Option 5: Move 1 missionary AND 1 cannibal over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.c < 1)
			{
				ErrorMessage("Insufficient cannibals: " + newState.across.c);
			}
			else
			{
				if (newState.across.m < 1)
				{
					ErrorMessage("Insufficient missionaries: " + newState.across.m);
				}
				else
				{
					newState.across.MoveMissionary(&newState.original, 1);
					newState.across.MoveCannibal(&newState.original, 1);
					newState.boatSide = !newState.boatSide;
				}
			}
		}
		else
		{
			if (newState.original.c < 1)
			{
				ErrorMessage("Insufficient cannibals: " + newState.original.c);
			}
			else
			{
				if (newState.original.m < 1)
				{
					ErrorMessage("Insufficient missionaries: " + newState.original.m);
				}
				else
				{
					newState.original.MoveMissionary(&newState.across, 1);
					newState.original.MoveCannibal(&newState.across, 1);
					newState.boatSide = !newState.boatSide;
				}
			}
		}
		break;

	case 6:
		cout << "Option 6: Move 1 missionary AND 2 cannibals over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.c < 2)
			{
				ErrorMessage("Insufficient cannibals: " + newState.across.c);
			}
			else
			{
				if (newState.across.m < 1)
				{
					ErrorMessage("Insufficient missionaries: " + newState.across.m);
				}
				else
				{
					newState.across.MoveMissionary(&newState.original, 1);
					newState.across.MoveCannibal(&newState.original, 2);
					newState.boatSide = !newState.boatSide;
				}
			}
		}
		else
		{
			if (newState.original.c < 2)
			{
				ErrorMessage("Insufficient cannibals: " + newState.original.c);
			}
			else
			{
				if (newState.original.m < 1)
				{
					ErrorMessage("Insufficient missionaries: " + newState.original.m);
				}
				else
				{
					newState.original.MoveMissionary(&newState.across, 1);
					newState.original.MoveCannibal(&newState.across, 2);
					newState.boatSide = !newState.boatSide;
				}
			}
		}
		break;
	case 7:
		cout << "Option 7: Move 2 missionaries AND 1 cannibal over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.c < 1)
			{
				ErrorMessage("Insufficient cannibals: " + newState.across.c);
			}
			else
			{
				if (newState.across.m < 2)
				{
					ErrorMessage("Insufficient missionaries: " + newState.across.m);
				}
				else
				{
					newState.across.MoveMissionary(&newState.original, 2);
					newState.across.MoveCannibal(&newState.original, 1);
					newState.boatSide = !newState.boatSide;
				}
			}
		}
		else
		{
			if (newState.original.c < 1)
			{
				ErrorMessage("Insufficient cannibals: " + newState.original.c);
			}
			else
			{
				if (newState.original.m < 2)
				{
					ErrorMessage("Insufficient missionaries: " + newState.original.m);
				}
				else
				{
					newState.original.MoveMissionary(&newState.across, 2);
					newState.original.MoveCannibal(&newState.across, 1);
					newState.boatSide = !newState.boatSide;
				}
			}
		}
		break;
	case 8:
		cout << "Option 8: Move 3 cannibals over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.c < 3)
			{
				ErrorMessage("Insufficient cannibals: " + newState.across.c);
			}
			else
			{
				newState.across.MoveCannibal(&newState.original, 3);
				newState.boatSide = !newState.boatSide;
			}
		}
		else
		{
			if (newState.original.c < 3)
			{
				ErrorMessage("Insufficient cannibals: " + newState.original.c);
			}
			else
			{
				newState.original.MoveCannibal(&newState.across, 3);
				newState.boatSide = !newState.boatSide;
			}
		}
		break;
	case 9:
		cout << "Option 9: Move 3 missionaries over" << endl;
		if (newState.boatSide)
		{
			if (newState.across.m < 3)
			{
				ErrorMessage("Insufficient missionaries: " + newState.across.m);
			}
			else
			{
				newState.across.MoveMissionary(&newState.original, 3);
				newState.boatSide = !newState.boatSide;
			}
		}
		else
		{
			if (newState.original.m < 3)
			{
				ErrorMessage("Insufficient missionaries: " + newState.original.m);
			}
			else
			{
				newState.original.MoveMissionary(&newState.across, 3);
				newState.boatSide = !newState.boatSide;
			}
		}
		break;

	}

	return newState;
}


bool Contains(vector<State>& v, State state)
{
	bool result = false;

	for (int i = 0; i < v.size(); i++)
	{
		if (v.at(i) == state)
		{
			result = true;
			break;
		}
	}

	return result;
}

int GetGoalNodePosition(vector<Node> f, State theGoal)
{
	int result = -1;

	for (int i = f.size() - 1; i >= 0; i--)
	{
		if (f.at(i).state == theGoal)
		{
			result = i;
			break;
		}
	}

	return result;
}