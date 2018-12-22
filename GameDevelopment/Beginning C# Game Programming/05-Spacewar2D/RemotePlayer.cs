using System;
using System.Net.Sockets;

namespace SpaceWar {
	/// <summary>
	/// Holds information about the other players in the game.
	/// </summary>
	public class RemotePlayer {
		Ship ship;
		int playerID;
		string hostName;
		DateTime updateTime;			// last tick count at update
		bool active = true;				// is this player active

		public RemotePlayer(int playerID, string hostName, Ship ship) {
			this.playerID = playerID;
			this.hostName = hostName;
			this.ship = ship;
		}

		public override string ToString() {
			return hostName;
		}

		public Ship Ship {
			get {
				return ship;
			}
		}

		public int PlayerID {
			get {
				return playerID;
			}
		}
		
		public DateTime UpdateTime {
			get {
				return updateTime;
			}
			set {
				updateTime = value;
			}
		}

		public bool Active {
			get {
				return active;
			}
			set {
				active = value;
			}
		}

	}
}
