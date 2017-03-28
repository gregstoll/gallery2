{*
 * $Revision: 17417 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <div class="giDescription">
    <p>
      {g->text text="Microsoft Windows comes with a nice feature that allows you to publish content from your desktop directly to a web service.  Follow the instructions below to enable this service on your Windows system."}
    </p>

    <h3>{g->text text="Installation"}</h3>
    <p>
      {g->text text="Download the configuration file using right-click 'Save Target As...'  Once downloaded, rename it to 'install_registry.reg'.  If it asks you for confirmation about changing the file type, answer 'yes'.  Right click on this file and you should see a menu appear.  Select the Merge option (this should be at the top of the menu).  It will ask you if you want to import these values into your registry.  Click 'Ok'.  It will tell you that the files were imported successfully.  Click 'Ok' again."}
    </p>
    {capture assign=vistaCaption}{g->text text="(for Windows Vista)"}{/capture}
    {capture assign=otherWindowsCaption}{g->text text="(for Windows XP, Windows 2000 and earlier Windows versions)"}{/capture}
    {capture assign=captionForRecommendedVersion}
      {if $ItemAddPublishXp.isUsingWindowsVista}
      {$vistaCaption}
      {else}
      {$otherWindowsCaption}
      {/if}
    {/capture}
    {capture assign=captionForAlternativeVersion}
      {if $ItemAddPublishXp.isUsingWindowsVista}
      {$otherWindowsCaption}
      {else}
      {$vistaCaption}
      {/if}
    {/capture}
    {capture assign=fileCaption}{g->text text="Download [install_registry.reg]"}{/capture}
    <ul>
      <li style="font-weight: bold; line-height: 1.2em; font-size: 1.2em">
        <a href="{g->url arg1="view=publishxp.DownloadRegistryFile" arg2="vistaVersion=`$ItemAddPublishXp.isUsingWindowsVista`"}">
          {$fileCaption}
        </a> {$captionForRecommendedVersion}
      </li>
      <li>
        <a href="{g->url arg1="view=publishxp.DownloadRegistryFile" arg2="vistaVersion=`$ItemAddPublishXp.isUsingOtherWindows`"}">
          {$fileCaption}
        </a> {$captionForAlternativeVersion}
      </li>
    </ul>

    <br/>
    {capture assign=instructionsVista}
    <h4>{g->text text="Windows Vista"}</h4>
    <ol>
      <li>{g->text text="Open your %sWindows Photo Gallery%s."
                   arg1='<a href="http://en.wikipedia.org/wiki/Windows_Photo_Gallery">'
                   arg2="</a>"}</li>
      <li>{g->text text="Select one or more images / files in your Windows Photo Gallery."}</li>
      <li>{g->text text='Click "Print..." and select the online printing option (not the local printing option).'}</li>
      <li>{g->text text="Select the Gallery 2 printing service."}</li>
      <li>{g->text text="Then follow the instructions to log into your Gallery, select an album and publish the image(s)."}</li>
    </ol>
    {/capture}
    {capture assign=instructionsOtherWindowsVersions}
    <h4>{g->text text="Windows XP, Windows 2000 and earlier Windows versions"}</h4>
    <ol>
      <li>{g->text text="Open your Windows Explorer and browse to a folder containing supported images."}</li>
      <li>{g->text text="Select the image(s) or a folder."}</li>
      <li>{g->text text='Click the link on the left that says "Publish this file to the web..."'}</li>
      <li>{g->text text="Select the Gallery 2 printing service."}</li>
      <li>{g->text text="Then follow the instructions to log into your Gallery, select an album and publish the image(s)."}</li>
    </ol>
    {/capture}
    {capture assign=instructionsForRecommendedVersion}
      {if $ItemAddPublishXp.isUsingWindowsVista}
      {$instructionsVista}
      {else}
      {$instructionsOtherWindowsVersions}
      {/if}
    {/capture}
    {capture assign=instructionsForAlternativeVersion}
      {if $ItemAddPublishXp.isUsingWindowsVista}
      {$instructionsOtherWindowsVersions}
      {else}
      {$instructionsVista}
      {/if}
    {/capture}
    <h3>{g->text text="Usage"}</h3>
    {$instructionsForRecommendedVersion}

    {$instructionsForAlternativeVersion}

  </div>
</div>
