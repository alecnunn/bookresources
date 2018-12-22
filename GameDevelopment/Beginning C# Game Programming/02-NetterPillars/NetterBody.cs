using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class NetterBody : Sprite {
		public NetterBody(bool isComputer) : base(Application.StartupPath+"\\"+IMAGE_PATH+"\\"+ (isComputer ? "" : "Player") +"NetterBody.gif") {        
		}
	}
}
