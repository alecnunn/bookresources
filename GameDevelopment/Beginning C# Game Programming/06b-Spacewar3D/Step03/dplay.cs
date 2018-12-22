using System;
using System.Collections;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;

/// <summary>
/// The class that houses DirectPlay code for the application.
/// </summary>
public class PlayClass : IDisposable
{
    private MessageDelegate message = null;

    private GameClass parent = null;

    public  Peer peerObject = null;
    private ConnectWizard Connect = null;
    private ArrayList PlayerList = new ArrayList();
    private int LocalPlayerID = 0;
    public Guid AppGuid = new Guid(0x876a3036, 0xffd7, 0x46bc, 0x92, 0x9, 0xb4, 0x2f, 0x61, 0x7b, 0x9b, 0xF1);

    public struct Players
    {
        public int dpnID;
        public string Name;
        public Players(int id, string n)
        { dpnID = id; Name = n; }
    }       

    public void Dispose()
    {
        peerObject.Dispose();
    }

    public PlayClass(GameClass parent)
    {
        this.parent = parent;
        this.peerObject = peerObject;
        this.message = new MessageDelegate(parent.MessageArrived);

        peerObject = new Peer();
        // First set up our event handlers (We only need events for the ones we care about)
        peerObject.PlayerCreated += new PlayerCreatedEventHandler(this.PlayerCreated);
        peerObject.PlayerDestroyed += new PlayerDestroyedEventHandler(this.PlayerDestroyed);
        peerObject.HostMigrated += new HostMigratedEventHandler(this.HostMigrated);
        peerObject.Receive += new ReceiveEventHandler(this.DataReceived);
        peerObject.SessionTerminated += new SessionTerminatedEventHandler(this.SessionTerminated);

        Connect = new ConnectWizard(peerObject, AppGuid, "Step2");
        if (!Connect.StartWizard())
        {
            MessageBox.Show("DirectPlay initialization was incomplete. Application will terminate.");
            throw new DirectXException();
        }
    }

    public void WriteMessage(byte msg)
    {
        NetworkPacket packet = new NetworkPacket();

        packet.Write(msg);
        peerObject.SendTo((int)PlayerID.AllPlayers, packet, 0, SendFlags.Guaranteed);
    }

    #region DirectPlayEvents
    /// <summary>
    // These are the events the app will handle
    // when DPlay fires them.
    /// </summary>
    private void PlayerCreated(object sender, PlayerCreatedEventArgs dpMessage)
    {
        // Get the PlayerInformation and store it 
        PlayerInformation dpPeer = peerObject.GetPeerInformation(dpMessage.Message.PlayerID);
        Players oPlayer = new Players(dpMessage.Message.PlayerID,dpPeer.Name);
        // We lock the data here since it is shared across multiple threads.
        lock (PlayerList)
        {
            PlayerList.Add(oPlayer);
        }
        // Save this player id if it's ourselves
        if (dpPeer.Local)
            LocalPlayerID = dpMessage.Message.PlayerID;

    }
    private void PlayerDestroyed(object sender, PlayerDestroyedEventArgs dpMessage)
    {
        // Remove this player from our list
        // We lock the data here since it is shared across multiple threads.
        lock (PlayerList)
        {
            foreach (Players player in PlayerList)
            {
                if (dpMessage.Message.PlayerID == player.dpnID)
                {
                    PlayerList.Remove(player);
                    break;
                }
            }
        }
    }
    private void HostMigrated(object sender, HostMigratedEventArgs dpMessage)
    {
        if (LocalPlayerID == dpMessage.Message.NewHostID) 
        {
            // This application instance is the new host, update the UI
            parent.Text += " (HOST)";
        }
    }
    private void DataReceived(object sender, ReceiveEventArgs dpMessage)
    {
        lock(this)
        {
            byte data = (byte)dpMessage.Message.ReceiveData.Read(typeof(byte));
            message(data);
        }
    }
    private void SessionTerminated(object sender, SessionTerminatedEventArgs dpMessage)
    {
        // This session is being terminated, let the user know
        if (dpMessage.Message.ResultCode == ResultCode.HostTerminatedSession)
            MessageBox.Show("The Host has terminated this session.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);
        else
            MessageBox.Show("The session has been lost.  This sample will now exit.", "Exiting", MessageBoxButtons.OK, MessageBoxIcon.Information);

        // This will post a message on the main thread to shut down our form
        parent.BeginInvoke(new PeerCloseCallback(parent.PeerClose));
    }
    #endregion
}