using System;
using Microsoft.DirectX;


	/// <summary>
	/// Holds information about the other players in the game.
	/// </summary>
public class RemotePlayer
{
	int playerID;
	string hostName;
	Vector3 startingPosition;
	DateTime updateTime;			// last tick count at update
	bool active = true;				// is this player active

	public RemotePlayer(int playerID, string hostName)
	{
		this.playerID = playerID;
		this.hostName = hostName;
	}

	public override string ToString()
	{
		return hostName;
	}

	public Vector3 StartingPosition
	{
		get
		{
			return startingPosition;
		}
		set
		{
			startingPosition = value;
		}
	}

	public string HostName
	{
		get
		{
			return hostName;
		}
		set
		{
			hostName = value;
		}
	}

	public int PlayerID
	{
		get
		{
			return playerID;
		}
		set
		{
			playerID = value;
		}
	}
		
	public DateTime UpdateTime
	{
		get
		{
			return updateTime;
		}
		set
		{
			updateTime = value;
		}
	}

	public bool Active
	{
		get
		{
			return active;
		}
		set
		{
			active = value;
		}
	}

}
