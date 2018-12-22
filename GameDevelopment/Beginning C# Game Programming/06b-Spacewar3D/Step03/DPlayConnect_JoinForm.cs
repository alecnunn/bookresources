//-----------------------------------------------------------------------------
// File: DPlayConnect_JoinForm.cs
//
// Desc: Application class for the DirectPlay samples framework.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Collections;
using System.Windows.Forms;
using System.Threading;
using System.Timers;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;




/// <summary>
/// This form will search for existing running samples and allow you to join them or will
/// allow you to create your own session as a host
/// </summary>
public class CreateJoinForm : System.Windows.Forms.Form
{
    private const int EnumExpireThreshold = 2000;
    private bool amJoining = false;




    /// <summary>
    /// Hold our information of hosts we found
    /// </summary>
    private struct FindHostsResponseInformation
    {
        public int LastFoundTime;
        public ApplicationDescription ApplicationDesc;
        public int RoundTripLatencyMs;
        public Address sender;
        public Address device;
        public override string ToString()
        {
            if (ApplicationDesc.MaxPlayers > 0)
                return ApplicationDesc.SessionName + " (" + ApplicationDesc.CurrentPlayers + "/" + ApplicationDesc.MaxPlayers + ") (" + RoundTripLatencyMs + "ms)";
            else
                return ApplicationDesc.SessionName + " (" + ApplicationDesc.CurrentPlayers + ") (" + RoundTripLatencyMs + "ms)";
        }
    };

    private Peer peer;
    private ConnectWizard connectionWizard;
    private Address deviceAddress;
    private Address hostAddress = null;
    private bool isSearching = false;
    private int findHostHandle = 0;
    private CreateSessionForm createSessionForm = null;
    private ResultCode resultCode;
    private ArrayList foundHosts = new ArrayList();
    private bool isConnected = false;
    private ManualResetEvent connectEvent = null;
    private System.Timers.Timer updateListTimer = null;
    private System.Timers.Timer connectTimer = null;

    private System.Windows.Forms.ListBox lstSession;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button btnCancel;
    private System.Windows.Forms.Button btnSearch;
    private System.Windows.Forms.Button btnJoin;
    private System.Windows.Forms.Button btnCreate;




    /// <summary>
    /// Constructor
    /// </summary>
    public CreateJoinForm(Peer peerObject, Address addressObject, ConnectWizard connectionWizard)
    {
        //
        // Required for Windows Form Designer support
        //
        InitializeComponent();
        peer = peerObject;
        this.connectionWizard = connectionWizard;
        this.Text = connectionWizard.SampleName + " - " + this.Text;
        deviceAddress = addressObject;
	
        //Set up the event handlers
        peer.FindHostResponse += new FindHostResponseEventHandler(FindHostResponseMessage);
        peer.ConnectComplete += new ConnectCompleteEventHandler(ConnectResult);
        peer.AsyncOperationComplete += new AsyncOperationCompleteEventHandler(CancelAsync);

        //Set up our timer
        updateListTimer = new System.Timers.Timer(300); // A 300 ms interval
        updateListTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.UpdateTimer);
        updateListTimer.SynchronizingObject = this;
        updateListTimer.Start();
        //Set up our connect timer
        connectTimer = new System.Timers.Timer(100); // A 100ms interval
        connectTimer.Elapsed += new System.Timers.ElapsedEventHandler(this.ConnectTimer);
        connectTimer.SynchronizingObject = this;
        // Set up our connect event
        connectEvent = new ManualResetEvent(false);

    }




    /// <summary>
    /// An asynchronous operation was cancelled
    /// </summary>
    private void CancelAsync(object sender, AsyncOperationCompleteEventArgs e)
    {
        if (e.Message.AsyncOperationHandle == findHostHandle)
        {
            findHostHandle = 0;
            btnSearch.Text = "Start Search";
            isSearching = false;
            btnCreate.Enabled = true;
            btnSearch.Enabled = true;
        }
    }




    /// <summary>
    /// A host was found and responded to our query
    /// </summary>
    private void FindHostResponseMessage(object sender, FindHostResponseEventArgs dpMessage)
    {
        // Now we need to add this to our list of available sessions
        SessionAdd(dpMessage.Message);
    }




    /// <summary>
    /// Add this session to our list
    /// </summary>
    /// <param name="dpMessage"></param>
    private void SessionAdd(FindHostsResponseMessage dpMessage)
    {
        FindHostsResponseInformation dpInfo = new FindHostsResponseInformation();

        dpInfo.ApplicationDesc = dpMessage.ApplicationDescription;
        dpInfo.device = dpMessage.AddressDevice;
        dpInfo.sender = dpMessage.AddressSender;
        dpInfo.RoundTripLatencyMs = dpMessage.RoundTripLatencyMs;
        dpInfo.LastFoundTime = Environment.TickCount;

        // Let's check the items first and see if this one already exists
        bool isFound = false;
	
        lock (foundHosts)
        {
            for (int i = 0; i < lstSession.Items.Count; i++)
            {
                if (dpInfo.ApplicationDesc.GuidInstance == ((FindHostsResponseInformation)lstSession.Items[i]).ApplicationDesc.GuidInstance)
                {
                    foundHosts[i] = dpInfo;
                    lstSession.Items[i] = dpInfo;
                    isFound = true;
                }
            }
            if (!isFound)
            {
                lstSession.Items.Add(dpInfo);
                foundHosts.Add(dpInfo);
            }
        }
    }




    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool Disposing)
    {
        base.Dispose(Disposing);

        if (isSearching)
        {
            if (findHostHandle != 0)
                peer.CancelAsyncOperation(findHostHandle);
            isSearching = !isSearching;
        }
        if (connectTimer != null)
            connectTimer.Dispose();
	
        if (updateListTimer != null)
            updateListTimer.Dispose();
    }




#region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
        this.btnJoin = new System.Windows.Forms.Button();
        this.lstSession = new System.Windows.Forms.ListBox();
        this.label1 = new System.Windows.Forms.Label();
        this.btnCreate = new System.Windows.Forms.Button();
        this.btnSearch = new System.Windows.Forms.Button();
        this.btnCancel = new System.Windows.Forms.Button();
        this.SuspendLayout();
        // 
        // btnJoin
        // 
        this.btnJoin.Enabled = false;
        this.btnJoin.Location = new System.Drawing.Point(7, 227);
        this.btnJoin.Name = "btnJoin";
        this.btnJoin.Size = new System.Drawing.Size(74, 27);
        this.btnJoin.TabIndex = 2;
        this.btnJoin.Text = "Join";
        this.btnJoin.Click += new System.EventHandler(this.btnJoin_Click);
        // 
        // lstSession
        // 
        this.lstSession.Location = new System.Drawing.Point(5, 37);
        this.lstSession.Name = "lstSession";
        this.lstSession.Size = new System.Drawing.Size(400, 186);
        this.lstSession.TabIndex = 1;
        this.lstSession.DoubleClick += new System.EventHandler(this.btnJoin_Click);
        this.lstSession.SelectedValueChanged += new System.EventHandler(this.SelectedChange);
        // 
        // label1
        // 
        this.label1.Location = new System.Drawing.Point(5, 13);
        this.label1.Name = "label1";
        this.label1.Size = new System.Drawing.Size(312, 16);
        this.label1.TabIndex = 0;
        this.label1.Text = "Select session to join, or click Create to start a new session.";
        // 
        // btnCreate
        // 
        this.btnCreate.DialogResult = System.Windows.Forms.DialogResult.Yes;
        this.btnCreate.Location = new System.Drawing.Point(84, 227);
        this.btnCreate.Name = "btnCreate";
        this.btnCreate.Size = new System.Drawing.Size(74, 27);
        this.btnCreate.TabIndex = 2;
        this.btnCreate.Text = "Create";
        this.btnCreate.Click += new System.EventHandler(this.btnCreate_Click);
        // 
        // btnSearch
        // 
        this.btnSearch.Location = new System.Drawing.Point(319, 7);
        this.btnSearch.Name = "btnSearch";
        this.btnSearch.Size = new System.Drawing.Size(86, 27);
        this.btnSearch.TabIndex = 2;
        this.btnSearch.Text = "Start Search";
        this.btnSearch.Click += new System.EventHandler(this.btnSearch_Click);
        // 
        // btnCancel
        // 
        this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnCancel.Location = new System.Drawing.Point(333, 227);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.Size = new System.Drawing.Size(74, 27);
        this.btnCancel.TabIndex = 3;
        this.btnCancel.Text = "Cancel";
        // 
        // CreateJoinForm
        // 
        this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
        this.CancelButton = this.btnCancel;
        this.ClientSize = new System.Drawing.Size(411, 264);
        this.ControlBox = false;
        this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                        this.btnCreate,
                                                                        this.btnJoin,
                                                                        this.btnSearch,
                                                                        this.btnCancel,
                                                                        this.lstSession,
                                                                        this.label1});
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "CreateJoinForm";
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
        this.Text = "Join or create a session";
        this.ResumeLayout(false);

    }
#endregion




    /// <summary>
    /// The selected session was changed.
    /// </summary>
    private void SelectedChange(object sender, System.EventArgs e)
    {
        if (amJoining)
            return; // Do nothing if we are already joining a session

        btnJoin.Enabled = (lstSession.SelectedItem != null);
    }




    /// <summary>
    /// We either want to start or stop searching
    /// </summary>
    private void btnSearch_Click(object sender, System.EventArgs e)
    {
        if (!isSearching)
        {
            if( hostAddress != null )
                hostAddress.Dispose();

            hostAddress = new Address();
            hostAddress.ServiceProvider = deviceAddress.ServiceProvider;

            // See if we should prompt the user for the remote address
            if (ConnectWizard.ProviderRequiresPort(hostAddress.ServiceProvider))
            {
                AddressForm addressDialog = new AddressForm(connectionWizard.DefaultPort);
                addressDialog.ShowDialog(this);

                // If the user cancelled the address form, abort the search
                if (addressDialog.DialogResult != DialogResult.OK)
                    return;

                // If a port was specified, add the component
                if (addressDialog.Hostname != "")
                    hostAddress.AddComponent(Address.KeyHostname, addressDialog.Hostname);
                
                // If a hostname was specified, add the component
                if (addressDialog.Port > 0)
                    hostAddress.AddComponent(Address.KeyPort, addressDialog.Port);
            }

            //Time to enum our hosts
            ApplicationDescription desc = new ApplicationDescription();
            desc.GuidApplication = connectionWizard.ApplicationGuid;
		    
            // If the user was not already asked for address information, DirectPlay
            // should prompt with native UI
            FindHostsFlags flags = 0;
            if (!ConnectWizard.ProviderRequiresPort(deviceAddress.ServiceProvider))
                flags = FindHostsFlags.OkToQueryForAddressing;

            peer.FindHosts(desc,hostAddress,deviceAddress,null,Timeout.Infinite,0,Timeout.Infinite, flags, out findHostHandle);
            isSearching = true;
            btnCreate.Enabled = false;
            btnSearch.Text = "Stop Search";
        }
        else
        {
            btnSearch.Text = "Stopping...";
            btnSearch.Enabled = false;
            if (findHostHandle != 0)
                peer.CancelAsyncOperation(findHostHandle);
        }
    }




    /// <summary>
    /// A form was disposed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    public void FormDisposed(object sender, EventArgs e)
    {
        if (sender == createSessionForm)
            createSessionForm = null;
    }




    /// <summary>
    /// We should create a new session.  Display a dialog allowing the user to set
    /// certain options
    /// </summary>
    private void btnCreate_Click(object sender, System.EventArgs e)
    {
        this.DialogResult = DialogResult.None;
        ((Button)sender).DialogResult = DialogResult.None;
        if (createSessionForm == null)
        {
            createSessionForm = new CreateSessionForm(peer,deviceAddress,connectionWizard);
            createSessionForm.Disposed += new System.EventHandler(this.FormDisposed);
        }
        if (createSessionForm.ShowDialog(this) != DialogResult.Cancel)
        {
            ((Button)sender).DialogResult = DialogResult.Yes;
            this.DialogResult = DialogResult.Yes;
        }
    }




    /// <summary>
    /// Determine if we need to expire any of our currently listed servers
    /// </summary>
    private void UpdateTimer(object sender, System.Timers.ElapsedEventArgs e)
    {
        lock (foundHosts)
        {
            for (int i = 0; i< foundHosts.Count; i++)
            {
                // First check to see if this session has expired
                if (Environment.TickCount - ((FindHostsResponseInformation)foundHosts[i]).LastFoundTime > EnumExpireThreshold)
                {
                    foundHosts.RemoveAt(i);
                    break;
                }
            }
            for (int i = 0; i< lstSession.Items.Count; i++)
            {
                // First check to see if this session has expired
                if (Environment.TickCount - ((FindHostsResponseInformation)lstSession.Items[i]).LastFoundTime > EnumExpireThreshold)
                {
                    lstSession.Items.RemoveAt(i);
                    break;
                }
            }
        }
    }




    /// <summary>
    /// Wait for a connect to complete
    /// </summary>
    private void ConnectTimer(object sender, System.Timers.ElapsedEventArgs e)
    {
        if (connectEvent.WaitOne(0,false)) // Wait for the Connect event to be fired
        {
            if (isConnected) // Are we connected?
            {
                // Get rid of the timer
                if (updateListTimer != null)
                    updateListTimer.Stop();

                if (connectTimer != null)
                    connectTimer.Stop();

                this.DialogResult = DialogResult.OK;
                this.Close();
                return;
            }
            else
            {
                MessageBox.Show(this,"Failed to connect.  The error code was: \n" + resultCode.ToString() ,"Failed to connect",MessageBoxButtons.OK,MessageBoxIcon.Information);
                // Restart our timer
                updateListTimer.Start();
            }
        }
    }




    /// <summary>
    /// Fired when a connect complete message is received from DirectPlay.  Fire
    /// our event to notify the sample we've connected
    /// </summary>
    private void ConnectResult(object sender, ConnectCompleteEventArgs e)
    {
        if (e.Message.ResultCode == 0)
        {
            isConnected = true;
        }
        else
            isConnected = false;

        resultCode = e.Message.ResultCode;
        // Notify the timer that we've connected.
        connectEvent.Set();
    }




    /// <summary>
    /// Join an existing session
    /// </summary>
    private void btnJoin_Click(object sender, System.EventArgs e)
    {
        FindHostsResponseInformation dpInfo;
        if (lstSession.SelectedItem == null)
        {
            MessageBox.Show(this,"Please select a session before clicking join.","No session",MessageBoxButtons.OK,MessageBoxIcon.Information);
            return;
        }

        // Turn off all buttons
        btnCreate.Enabled = false;
        btnCancel.Enabled = false;
        btnJoin.Enabled = false;
        btnSearch.Enabled = false;
        amJoining = true;
        // Stop our secondary timer
        updateListTimer.Stop();
        dpInfo = ((FindHostsResponseInformation)lstSession.SelectedItem);
        connectionWizard.SetUserInfo();
        peer.Connect(dpInfo.ApplicationDesc,dpInfo.sender, dpInfo.device,null,ConnectFlags.OkToQueryForAddressing);
        // Now start our 'Connect' timer
        connectTimer.Start();
    }
}
