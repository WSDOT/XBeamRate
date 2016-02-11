///////////////////////////////////////////////////
// 
// RunRegression.js 
// A script for running XBRate regression tests.
// Run with /? to get help.
//
//
///////////////////////////////////////////////////

// first some defaults
var XBRateVersion = "Debug";
var XBRatePlatform = "WIN32";
var DoSendEmail=false;
var EmailAddress = new String;
var ExecuteCommands=true; // if false, only show commands

var startDate = new Date();
var startTime = startDate.getTime();

// hack for convenience:
// set up drives so pickings' and brice's machines work without command line options
var XBRateDrive = "C:";
var objNet = new ActiveXObject( "WScript.Network" );
var machine = objNet.ComputerName;
machine = machine.toUpperCase();
if (machine=="RDPTHINKPAD")
   XBRateDrive = "C:";
else if (machine=="HQB0630025")
   XBRateDrive = "F:";
else if (machine=="HQA4434036")
   XBRateDrive = "F:";
else if (machine=="HQC1431001")
   XBRateDrive = "F:";
else if (machine=="HQD1764064")
   XBRateDrive = "F:";

var wsShell = new ActiveXObject("WScript.Shell");
var FSO = new ActiveXObject("Scripting.FileSystemObject");

// Create file name for timer file
var timerFileDate = new String;
timerFileDate = startDate.toString();
timerFileDate = timerFileDate.replace(/ /gi, "_"); // regular expression pinta to make a reasonable file name
timerFileDate = timerFileDate.replace(/:/gi, ";");
var timerFileName = new String;
timerFileName = "\\ARP\\XBeamRate\\RegressionTest\\RegTimer_" + timerFileDate + ".log";

// parse the command line and set options
var st = ParseCommandLine();

if (ExecuteCommands) 
{
    var timerFile = FSO.OpenTextFile(timerFileName, 2, true); // 2 = write new file
    timerFile.WriteLine("********** Starting new regr test at: " + startDate.toString() + " elapsed times in minutes *************");
}

if (st!=0)
{
   CleanUpTest();
   WScript.Quit(st);
}

// global vars
var OldCatalogServer = new String;
var NewCatalogServer = new String("Regression");
var OldCatalogPublisher = new String;
var NewCatalogPublisher = new String("Regression");

var Application = XBRateDrive+"\\ARP\\BridgeLink\\RegFreeCOM\\" +XBRatePlatform+"\\"+XBRateVersion+"\\BridgeLink.exe ";
var StartFolderSpec = new String( XBRateDrive+"\\ARP\\XBeamRate\\RegressionTest" );

var ErrorsExist = "FALSE";

// make sure exe exists
if (!FSO.FileExists(Application))
{
   DisplayMessage("Error - Program File: "+Application+" does not exist - Script Terminated");
   CleanUpTest();
   WScript.Quit(1);
}

// First clean up results from any old runs and set up environment
var CurrentFolder = StartFolderSpec;
InitTest(CurrentFolder);

var CurrCommand="XBRTest";
RunTest(CurrentFolder,CurrCommand);

if(ExecuteCommands)
{
    CheckResults(CurrentFolder);
}

CleanUpTest();

var st = 0;
if (!ErrorsExist)
{
   DisplayMessage("Test Successful - no errors");
   st = 1;
}

var endDate = new Date();
var endTime = endDate.getTime();
var elapsed = (endTime-startTime)/60000.0;
DisplayMessage("Elapsed Time was: "+elapsed+" Minutes");

if (ExecuteCommands) {
// only log times if the tess were actually run... (don't log times when /N option is used... it isn't meaninful data)
    var myFile = FSO.OpenTextFile("\\ARP\\XBeamRate\\RegressionTest\\RegTest.log", 8, true); // 8 = ForAppending (for some reason the ForAppending constant isn't defined)
    myFile.WriteLine(endDate.toString() + " : Elapsed Time was: "+elapsed+" Minutes");
    myFile.close();

    timerFile.WriteLine(endDate.toString() + " : Total Elapsed Time was: " + elapsed + " Minutes");
    timerFile.close();
}
      
WScript.Quit(st);

// --------------------- functions ------------------
function RunTest (currFolder, currCommand) 
{
   // find all .xbr files and run XBRate on them
   var Folder = FSO.GetFolder(currFolder);
   var ff = new Enumerator(Folder.SubFolders);
   for (; !ff.atEnd(); ff.moveNext())
   {
      var folderName = new String(ff.item());

      // Don't walk into forbidden 
      if (IsForbiddenFolder(folderName))
      {
         continue;
      }

      var subFolder = FSO.GetFolder(ff.item());

      // Need to parse current command here if in folder name
      var newCommand = ParseCommandFromFolderName(currCommand, subFolder.Name);

      if (newCommand == "XBRTest") 
      {
          var fc = new Enumerator(subFolder.Files);
          for (; !fc.atEnd(); fc.moveNext()) {
              s = new String(fc.item());

              idx1 = s.indexOf(".xbr");
              idx2 = s.indexOf(".pgs");
              idx3 = s.indexOf(".spl");
              if (idx1 != -1 || idx2 != -1 || idx3 != -1) {
                  cmd = Application + " /" + newCommand + " " + s;

                  if (ExecuteCommands) {
                      var begDate = new Date();
                      var begrunTime = begDate.getTime();

                      DisplayMessage("Running: " + cmd);
                      DisplayMessage("");
                      st = wsShell.Run(cmd, 1, "TRUE");

                      var endrunDate = new Date();
                      var endrunTime = endrunDate.getTime();
                      var elapsed = (endrunTime - begrunTime) / 60000.0;
                      timerFile.WriteLine(s + ", " + elapsed); // comma-delimited elapsed time for each file
                  }
                  else {
                      DisplayMessage(cmd);
                  }
              }
          }
      }  

     // Recurse
     RunTest(subFolder,newCommand);
   }
}

function InitTest(currFolder) 
{
    if (ExecuteCommands)
    {
       // clean up temporary files - only when actual test
       CleanFolder(currFolder);
    }
   // Save initial server and publisher
   OldCatalogServer = wsShell.RegRead("HKEY_CURRENT_USER\\Software\\Washington State Department of Transportation\\PGSuper\\Options\\CatalogServer2");
   OldCatalogPublisher = wsShell.RegRead("HKEY_CURRENT_USER\\Software\\Washington State Department of Transportation\\PGSuper\\Options\\Publisher2");
   
   // Run PGSuper to set new server and publisher
   SetPGSuperLibrary(NewCatalogServer, NewCatalogPublisher);
}   

function SetPGSuperLibrary(server, publisher)
{
   var cmd = new String;
   cmd = Application + "/App=PGSuper /Configuration=\"" + server + "\":\"" + publisher + "\"";

   if(ExecuteCommands)
   {
       DisplayMessage("Running: "+ cmd);
       DisplayMessage("");
       st = wsShell.Run(cmd,1,"TRUE"); 
   }
   else
   {
       DisplayMessage(cmd);
   }
}

function CleanFolder(currFolder)
{
   var Folder = FSO.GetFolder(currFolder);
   var ff = new Enumerator(Folder.SubFolders);
   for (; !ff.atEnd(); ff.moveNext())
   {
      var folderName = new String(ff.item());

      // Don't walk into forbidden folders
      if (IsForbiddenFolder(folderName))
      {
         continue;
      }

      var subFolder = FSO.GetFolder(ff.item());

      var fc = new Enumerator(subFolder.Files);
      for (; !fc.atEnd(); fc.moveNext())
      {
        s = new String(fc.item());
        s = s.toLowerCase();
        if ( -1!=s.indexOf(".err") || -1!=s.indexOf(".test") || -1!=s.indexOf(".dbr") || -1!=s.indexOf(".dbp") || -1!=s.indexOf(".log") )
        {
           FSO.DeleteFile(s);
        }

      }

      // recurse to subfolders
      CleanFolder(subFolder);
   }
}

function CleanUpTest() 
{
   // Restore registry to where it was before we started
   // Run PGSuper to set new server and publisher
   SetPGSuperLibrary(OldCatalogServer, OldCatalogPublisher);
}

function CheckResults(currFolder) 
{
   // first check to see if any .err files exist.
   // clean up empty error files 

   var Folder = FSO.GetFolder(currFolder);
   var ff = new Enumerator(Folder.SubFolders);
   for (; !ff.atEnd(); ff.moveNext())
   {
      var folderName = new String(ff.item());

      // Don't walk into forbidden folders
      if (IsForbiddenFolder(folderName))
      {
         continue;
      }

      var subFolder = FSO.GetFolder(ff.item());

      var fc = new Enumerator(subFolder.Files);
      for (; !fc.atEnd(); fc.moveNext())
      {
        s = new String(fc.item());
        s = s.toLowerCase();
        if ( -1!=s.indexOf(".err") )
        {
           fil = fc.item();
           if (fil.size == 0)
           {
               FSO.DeleteFile(fil);
           }
           else
           {
               ErrorsExist="TRUE";
               DisplayMessage("Test Failed - .err files exist in: "+subFolder);
               continue;
           }
        }

      }

      CheckResults(subFolder) 
   }
  
}


function ParseCommandFromFolderName(currCommand, folderName)
{
  var idx = folderName.indexOf("-")
  if ( -1 != idx)
  {
     // We have a -, so parse command to end of name
     var s = folderName.substr(idx+1); 
     s = s.toUpperCase();

     var s3 = s.substr(0,3); // take first three characters to check for Tx commands
     var s4 = s.substr(0,4); // take first four characters to check for Tx commands
     var cmd;

//     if (s4=="TXAT")
//     {
//        cmd = "TxAT";
//     }
//     else if (s4=="TXDT")
//     {
//        cmd = "TxDT";
//     }
//     else if (s4 == "TXDS") {
//         cmd = "TxDS";
//     }
//     else if (s4 == "TXDX")
//     {
//        cmd = "TxDx";
//     }
//     else if (s=="TESTDF")
//     {
//        cmd = "TestDF";
//     }
//     else if (s3=="125")
//     {
//        cmd = "TestR"; // only full regression for now
//     }
//     else if (s=="TXTOGA")
//     {
//        cmd = "TxToga";
//     }
//     else if ( s == "XBRTEST" )
//     {
//        cmd = "XBRTest"
//     }
//     else
//     {
         DisplayMessage("Error - Invalid Folder Name, could not parse command: "+s);
         CleanUpTest();
         WScript.Quit(1);
//     }

     return cmd;
     
  }
  else
  {
     // No new command, use old one
     return currCommand;
  }
}

function ParseCommandLine()
{
   var didDrive=0; // drive on command line?
   var didVersion=0; // Release or Debug on command line?
   var didPlatform=0;
   var objArgs = WScript.Arguments;
   var I;
   for (I=0; I<objArgs.Count(); I++)
   {
      var s = new String(objArgs(I));
        
      if (s.charAt(0)=="/" || s.charAt(0)=="-")
      {
       if (s.charAt(1)=="?")
       {
         DisplayMessage("--- Help for Test.js - Regression Testing Script ---");
         DisplayMessage("Command Line options:");
         DisplayMessage("    /?             - Help (you are here)");
         DisplayMessage("    /D<drive-name> - Drive where XBRate is installed (e.g., /DC:)");
         DisplayMessage("    /V<version>    - Version of XBRate to test (either \"Debug\", \"Profile\", or \"Release\"");
         DisplayMessage("    /P<platform>   - Platform (Win32 or X64)");
         DisplayMessage("    /N             - No execute. Display but do not execute XBRate commands.");
         DisplayMessage("");
         return 1;
       }
       else if (s.charAt(1)=="D" || s.charAt(1)=="d")
       {
          var dr = s.substring(2,s.length);
          dr = dr.toUpperCase();
          if (dr.length==1 && dr.charAt(1)!=":")
          {
             DisplayMessage("Invalid format for Drive Letter - must be in form C:");
             DisplayMessage("String was: \""+dr+"\"");
             return 1;
          }
          XBRateDrive = dr;
          didDrive=1;          
       }
       else if (s.charAt(1)=="V" || s.charAt(1)=="v")
       {
          var ver = s.substring(2,s.length);
          ver = ver.toUpperCase();
          if (ver=="DEBUG" || ver=="RELEASE"|| ver=="PROFILE")
          {
             XBRateVersion = ver;
             didVersion=1;
          }
          else
          {
             DisplayMessage("Invalid XBRate version on command line - must be either Release, Profile, or Debug");
             DisplayMessage("String was: \""+ver+"\"");
             return 1;
          }
       }
       else if (s.charAt(1)=="P" || s.charAt(1)=="p")
       {
          var platform = s.substring(2,s.length);
          platform = platform.toUpperCase();
          if (platform =="WIN32" || platform =="X64")
          {
             XBRatePlatform = platform;
             didPlatform=1;
          }
          else
          {
             DisplayMessage("Invalid platform on command line - must be either Win32 or x64");
             DisplayMessage("String was: \""+platform+"\"");
             return 1;
          }
       }
       else if (s.charAt(1)=="S" || s.charAt(1)=="s")
       {
          // set up for sending email
          EmailAddress = s.substring(2,s.length);
          DoSendEmail = true;
       }
       else if (s.charAt(1)=="N" || s.charAt(1)=="n")
       {
          // No execute. Display but do not execute commands.
          ExecuteCommands=false;
       }
       else
       {
         DisplayMessage("Error - Bad Command line parameter"+s);
         return 1;
       }
     }
     else
     {
        DisplayMessage("Error - Bad Command line parameter: \""+s+"\"");
        return 1;
     }
   }
   
   return 0;
}

function SendEmail(status)
{
    // latch onto Outlook - error will be catastrophic
    var ol = new ActiveXObject("Outlook.Application");

    // Return a reference to the MAPI layer
    var ns = ol.GetNamespace("MAPI");

    // Create a new mail message item
    var newMail = ol.CreateItem(0); // olMailItem from typelib - dangerous, but only way we can play in jscript

    // Add the subject of the mail message
    if (st==0)
       newMail.Subject = "XBRate Regression Test - Passed";
    else
       newMail.Subject = "XBRate Regression Test - Failed";

    // Create some body text
    newMail.Body = "See attached diff file for more information";

    // Add  recipient and test to make sure that the address is valid using the Resolve method
    var rec = newMail.Recipients.Add(EmailAddress);
    rec.Type = 1; // olTo
    if (! rec.Resolve )
    {
        DisplayMessage( "Unable to resolve address.");
    }

    // Attach diff file to message
    var attch = newMail.Attachments.Add(DiffFile, 1); //olByValue 
    attch.DisplayName = "Diff File";

    //Send the mail message
    newMail.Send();
}

function IsForbiddenFolder(folderName)
{
   // Don't walk into cvs or special folders
   folderName = folderName.toUpperCase();
   var matchcvs = folderName.match("CVS");
   var matchdtm = folderName.match("DATUM");
   var match125 = folderName.match("1250_Diff");
   if (null!=matchcvs || null!=matchdtm || null!=match125)
   {
      return true;
   }

   return false;
}

function DisplayMessage(msg)
{
   // could make this modal and add dialog-type messages for interactive mode
   WScript.Echo(msg);
}

