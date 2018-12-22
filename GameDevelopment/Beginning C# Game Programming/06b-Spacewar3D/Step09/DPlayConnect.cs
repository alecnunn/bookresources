//-----------------------------------------------------------------------------
// File: DPlayConnect.cs
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
using System.Runtime.InteropServices;




/// <summary>
/// The connection wizard will allow our users to pick a service provider,
/// and then connect to an existing application, or create a new one as the host.
/// </summary>
public class ConnectWizard {
	private Peer peerObject;
	private Address deviceAddress;

	private ChooseServiceProviderForm serviceProviderForm;
	private CreateJoinForm createJoinForm;

	private Guid serviceProviderGuid;
	private string username;
	private string samplename;
	private bool isInSession = false;
	private bool isHost = false;
	private int port = 0;

	private Guid applicationGuid;




	/// <summary>
	/// Constructor
	/// </summary>
	/// <param name="peer"></param>
	/// <param name="applicationGuid"></param>
	/// <param name="sampleName"></param>
	public ConnectWizard(Peer peer, Guid application, string sampleName) {
		peerObject = peer;
		applicationGuid = application;
		samplename = sampleName;
	}




	/// <summary>
	/// The guid for the current service provider
	/// </summary>
	public Guid ServiceProvider {
		get {
          return serviceProviderGuid; }
		set {
          serviceProviderGuid = value; }
	}




	/// <summary>
	/// The game's default port number
	/// </summary>
	public int DefaultPort {
		get {
          return port; }
		set {
          port = value; }
	}




	/// <summary>
	/// Am I the host
	/// </summary>
	public bool IsHost {
		get {
          return isHost; }
		set {
          isHost = value; }
	}




	/// <summary>
	/// The username
	/// </summary>
	public string Username {
		get {
          return username; }
		set {
          username = value; }
	}




	/// <summary>
	/// The sample name
	/// </summary>
	public string SampleName {
		get {
          return samplename; }
	}




	/// <summary>
	/// The applications guid
	/// </summary>
	public Guid ApplicationGuid {
		get {
          return applicationGuid; }
	}




	/// <summary>
	/// Are we in a session
	/// </summary>
	public bool InSession {
		get { return isInSession; }
		set { isInSession = value; }
	}




	/// <summary>
	/// Returns true if the given provider requires a port component
	/// </summary>
	/// <param name="provider">ServiceProvider Guid</param>
	/// <returns></returns>
	static public bool ProviderRequiresPort(Guid provider) {
		return (provider != Address.ServiceProviderSerial &&
			provider != Address.ServiceProviderModem &&
			provider != Address.ServiceProviderBlueTooth);
	}




	/// <summary>
	/// Handler for when our form is disposed
	/// </summary>
	public void FormDisposed(object sender, EventArgs e) {
		if (sender == createJoinForm)
			createJoinForm = null;

		if (sender == serviceProviderForm)
			serviceProviderForm = null;
	}




	/// <summary>
	/// Set the user information 
	/// </summary>
	public void SetUserInfo() {
		//Before we call host, let's actually call SetPeerInformation
		PlayerInformation myinformation = new PlayerInformation();
		myinformation.Name = username;

		peerObject.SetPeerInformation(myinformation, SyncFlags.PeerInformation);
	}




	/// <summary>
	/// Show the service providers form
	/// </summary>
	/// <returns>True if a service provider was picked, false otherwise</returns>
	public bool DoShowServiceProviders() {
		if (serviceProviderForm == null) {
			username = null;
			serviceProviderForm = new ChooseServiceProviderForm(peerObject,this);
			serviceProviderForm.Disposed += new System.EventHandler(this.FormDisposed);
		}
		if (serviceProviderForm.ShowDialog() == DialogResult.OK)
			return true;

		// The didn't hit ok
		return false;
	}




	/// <summary>
	/// Show the create or join screen
	/// </summary>
	/// <returns>True if we will be in a session, false otherwise</returns>
	public bool DoCreateJoinGame() {
		if (deviceAddress != null)
			deviceAddress.Dispose();

		deviceAddress = new Address();

		if (createJoinForm != null)
			createJoinForm.Dispose();

		createJoinForm = new CreateJoinForm(peerObject, deviceAddress, this);
		createJoinForm.Disposed += new System.EventHandler(this.FormDisposed);

		//Set the address's service provider (this will be the device address)
		deviceAddress.ServiceProvider = serviceProviderGuid;
		DialogResult drCreateJoin = createJoinForm.ShowDialog();
		if (drCreateJoin == DialogResult.Cancel)
			return false;

		this.isHost = (drCreateJoin == DialogResult.Yes);
		return true;

	}




	/// <summary>
	/// Start the wizard
	/// </summary>
	/// <returns>True if we are in a session, false otherwise</returns>
	public bool StartWizard() {
		isInSession = false;

		while (this.DoShowServiceProviders()) {
			//Now let's create a game or join a session
			if (this.DoCreateJoinGame()) {
				// A game has been created or joined now, set our flag
				isInSession = true;
				break;
			}
		}
		return isInSession;
	}
}


