{*
 * $Revision: 1264 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}

<div class="gbBlock gcBackground1">
    <h2>{g->text text="Map Module Settings"}   <a style="position:relative;left:50px;border-top:2px solid #cecece; border-left:2px solid #cecece; border-bottom:2px solid #4a4a4a;border-right:2px solid #4a4a4a;padding:.2em;padding-right:1em;padding-left:1em;text-decoration:none;background-color:#ebebeb;color:#000;font-weight:normal;font-size:12px;" href="{g->url arg1="view=mapv3.ShowMap"}">{g->text text="Show Google Map"}</a></h2>
</div>

<!--
Beginning of error/success displaying
-->
{if isset($status.saved)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Settings have been saved!"}</h2>
    </div>
{/if}
{if isset($status.generated)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Markers have been generated succesfully!"}</h2>
        <h2 class="giWarning">{g->text text="Save settings before continuing!"}</h2>
    </div>
{/if}
{if isset($status.deleted)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="The Marker Set has been deleted, it can be recreated via the Add button"}</h2>
        <h2 class="giWarning">{g->text text="Save settings before continuing!"}</h2>
    </div>
{/if}
{if !empty($form.error)}
<div class="gbBlock">
   <h2 class="giError">{g->text text="There was a problem processing your request."}</h2>
</div>
{/if}
{if isset($form.error.createdir)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="Cannot create the marker directory, check permissions."}</h2>
    </div>
{/if}
{if isset($form.createicons)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="Unknown error creating the markers!"}</h2>
    </div>
{/if}
{if isset($form.error.nofileselected)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="Please select a file to upload!"}</h2>
    </div>
{/if}
{if isset($form.error.badfileselected)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="Please select a PNG file to upload!"}</h2>
    </div>
{/if}
{if isset($form.error.notuploaded)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="File not uploaded, check that PHP supports file upload"}</h2>
    </div>
{/if}
{if isset($form.error.rightserror)}
    <div class="gbBlock">
        <h2 class="giError">{g->text text="File not uploaded, check the rights on the map/images folder and subfolders"}</h2>
    </div>
{/if}
{if isset($status.uploaded)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="File successfully uploaded"}</h2>
    </div>
{/if}
<!--
End of error/success displaying
-->

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="Markers"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->

<!--
Markers settings
-->

{if $form.mode eq ''}
<div class="gbBlock">
  <h3> {g->text text="Markers"}</h3>

  <p class="giDescription">
    {g->text text="This section allows you to manage marker settings"}
  </p>
      <table class="gbDataTable">
        <tr>
            <th><label for="ChooseMarkerSet">{g->text text="Manage marker sets"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_Manage,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
               {if ((isset($form.nomarkers)) or (isset($form.error.nomarkers)))}
                 <div class="gbBlock">
                    <h2 class="giError">{g->text text="Please create some markers"} -&gt;</h2>
                 </div>
               {/if}
               {if !(isset($form.nomarkers))}
               <input type="submit" name="{g->formVar var="form[action][viewall]"}" value="{g->text text="View All Available"}"/>
               {/if}
               <input type="submit" name="{g->formVar var="form[action][add]"}" value="{g->text text="Add" hint="Create new"}"/>
                {if !(isset($form.nomarkers))}
               <input type="submit" name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
               <select name="{g->formVar var="form[deleteMarkerSet]"}" value="{$form.deleteMarkerSet}">
                   {$form.markerset}
               </select>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="ChooseMarkerSet">{g->text text="Select default photo marker set and color"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_DefPhoto,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                {if !(isset($form.nomarkers))}
               <select name="{g->formVar var="form[useMarkerSet]"}" value="{$form.useMarkerSet}">
                   {$form.markerset}
               </select>
               {/if}
               {if ((isset($form.nomarkers)) or (isset($form.error.nomarkers)))}
                 <div class="gbBlock">
                    <h2 class="giError">{g->text text="You need to create some markers first."}</h2>
                 </div>
               {/if}
                {if !(isset($form.nomarkers)) and ((!isset($form.useMarkerSet)) or $form.useMarkerSet=='')}
                  <div class="gbBlock">
                     <h2 class="giError">{g->text text="Please select a default marker set and save."}</h2>
                  </div>
                {/if}
                {if !(isset($form.nomarkers))}
                <select name="{g->formVar var="form[defaultphotocolor]"}" value="{$form.defaultphotocolor}">
                    {$form.colorPoption}
                </select>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="ChooseMarkerSet">{g->text text="Select default album marker set and color"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_DefAlbum,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                 {if !(isset($form.nomarkers))}
               <select name="{g->formVar var="form[useAlbumMarkerSet]"}" value="{$form.useAlbumMarkerSet}">
                   {$form.albummarkerset}
               </select>
               {/if}
                {if !(isset($form.nomarkers)) and ((!isset($form.useAlbumMarkerSet)) or $form.useAlbumMarkerSet=='')}
                  <div class="gbBlock">
                     <h2 class="giError">{g->text text="Please select a default marker set and save."}</h2>
                  </div>
                {/if}
                {if !(isset($form.nomarkers))}
                <select name="{g->formVar var="form[defaultalbumcolor]"}" value="{$form.defaultalbumcolor}">
                    {$form.colorAoption}
                </select>
                {/if}
                {if ((isset($form.nomarkers)) or (isset($form.error.nomarkers)))}
                  <div class="gbBlock">
                     <h2 class="giError">{g->text text="You need to create some markers first."}</h2>
                  </div>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="ChooseMarkerSet">{g->text text="Select default group marker set and color"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_DefGroup,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                {if !(isset($form.nomarkers))}
               <select name="{g->formVar var="form[useGroupMarkerSet]"}" value="{$form.useGroupMarkerSet}">
                   {$form.groupmarkerset}
               </select>
               {/if}
               {if ((isset($form.nomarkers)) or (isset($form.error.nomarkers)))}
                 <div class="gbBlock">
                    <h2 class="giError">{g->text text="You need to create some markers first."}</h2>
                 </div>
               {/if}
                {if !(isset($form.nomarkers)) and ((!isset($form.useGroupMarkerSet)) or $form.useGroupMarkerSet=='')}
                  <div class="gbBlock">
                     <h2 class="giError">{g->text text="Please select a default marker set and save."}</h2>
                  </div>
                {/if}
                {if !(isset($form.nomarkers))}
                <select name="{g->formVar var="form[defaultgroupcolor]"}" value="{$form.defaultgroupcolor}">
                    {$form.colorGoption}
                </select>
                {/if}
            </td>
        </tr>
        <tr>
            <th><label for="useParentColor">{g->text text="Use parent album c<u>o</u>lor for items with no color setting"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_UseParent,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="o" type="checkbox" id="useParentColor"
                    name="{g->formVar var="form[useParentColor]"}"
                    {if $form.useParentColor}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
   </table>
</div>

<div class="gbBlock">
  <h3> {g->text text="Automatic Regroup Settings"}</h3>

  <p class="giDescription">
    {g->text text="This section allows you to use the regroup feature"}
  </p>
      <table class="gbDataTable">
        <tr>
            <th><label for="regroupAlbums">{g->text text="Automatic <u>a</u>lbum based regroup"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_AlbumRegroup,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="a" type="checkbox" id="regroupAlbums"
                    name="{g->formVar var="form[regroupAlbums]"}"
                    {if $form.regroupAlbums}checked="checked"{/if}
                    value="1"/>
            </td>
        </tr>
        <tr>
            <th><label for="regroupItems">{g->text text="Automatic <u>i</u>tem regroup"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_ItemRegroup,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input accesskey="i" type="checkbox" id="regroupItems"
                    name="{g->formVar var="form[regroupItems]"}"
                    {if $form.regroupItems}checked="checked"{/if}
                    value="1" onclick="ToggleInputs(['regroupIcon']);"/>
            </td>
        </tr>
        <tr>
            <th><label for="RegroupDist">{g->text text="Item regroup distance"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_Distance,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
            <td>
                <input type="text" id="regroupDist"
                    name="{g->formVar var="form[regroupDist]"}"
                    value="{$form.regroupDist}" maxlength="2" size="2"/>
                {if isset($form.error.regroupDist)}
                    <span class="giError">
                        {g->text text="Please specify a positive integer."}
                    </span>
                {/if}
            </td>
        </tr>
    <tr>
      <th><label>{g->text text="Upload a new Icon"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_UploadNew,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
         <td>
         <input type="file" name="{g->formVar var="form[2]"}"/>
         <input type="submit" name="{g->formVar var="form[action][uploadicon]"}" value="{g->text text="Upload a new Icon"}" />
      </td>
    </tr>
    <tr>
      <th>
        <label for="RegroupIcon">{g->text text="Icon for regrouped markers"}:</label>
        {if isset($form.error.marker)}
            <br\><span class="giError">
                {g->text text="Please select an icon."}
            </span>
        {/if}
      </th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_M_IconForRegroup,500)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
      <td>
      <table border=1 style='border-collapse:collapse;'>
        <tr>
        {foreach from=$form.multimarkers key=picname item=size}
          <td align=center><input {if !$form.regroupItems}disabled="true"{/if} id="itemicon" type=radio name="regroupIcon" value="{$picname}" {if $form.regroupIcon eq $picname}checked="checked"{/if}/></td>
        {/foreach}
        </tr><tr>
        {foreach from=$form.multimarkers key=picname item=size}
          <td align=center><img src={$size.2}/></td>
        {/foreach}
        </tr>
      </table>
      </td>
    </tr>
  </table>
</div>

<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>
{/if}

<!--
Displaying all the MarkerSet already created
-->
{if $form.mode eq 'ViewMarkers'}
<div class="gbBlock">
 <h3> {g->text text="View All Available MarkerSets"} </h3>

 <p class="giDescription">
   {g->text text="This section shows all available MarkerSets"}
 </p>
    {if (isset($form.nomarkers))}
    <div class="gbBlock">
       <h2 class="giError">{g->text text="There are no MarkerSets to display, how did you get here?"}</h2>
    </div>
    {/if}
    {if !(isset($form.nomarkers))}
     <table class="gbDataTable" style='border-collapse:collapse;'>
         {foreach from=$form.allmarkers key=num item=imagelist}
         {foreach from=$imagelist key=name item=images}
          <tr>
           <th><label for="{$name}">{$name}:</label></th>
           <td>
            {foreach from=$images item=image}{$image}{/foreach}
           </td>
          </tr>
          {/foreach}
          {/foreach}
    </table>
    {/if}
 </div>
<div class="gbBlock gcBackground1">
   <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Return" hint="Go back"}"/>
</div>
{/if}


<!--
Displaying the marker creation tool when requested
-->
{if $form.mode eq 'Tool'}
<div class="gbBlock">
  <h3> {g->text text="Markers Creation Tool"} </h3>

  <p class="giDescription">
    {g->text text="This section permits the creation of new markers in various sizes, colors and aspects"}
  </p>
  {if $form.noimagemagick}
  <div class="gbBlock">
     <h2 class="giWarning">{g->text text="ImageMagick wasn't detected as installed, you may experience problem generating markers"}</h2>
  </div>
  {/if}
  {if isset($form.nobase)}
  <div class="gbBlock">
     <h2 class="giError">{g->text text="Your BaseMarker folder does not exist, please check the module installation."}</h2>
  </div>
  {/if}

  {if !(isset($form.nobase))}
  <table class="gbDataTable">
      <tr>
        <th>
          <label>{g->text text="Base Image"}</label>
          {if isset($form.error.base)}
              <br\><span class="giError">
                  {g->text text="Please select a base style to use"}
              </span>
          {/if}
        </th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MC_Base,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <table border=1 style='border-collapse:collapse;'><tr>
          {foreach from=$form.basemarkers key=picname item=size}
            <td align=center><input type=radio name="{g->formVar var="form[base]"}" value='{$picname}'/></td>
          {/foreach}</tr><tr>
          {foreach from=$form.basemarkers key=picname item=size}
            <td align=center><img src={$size.2}/></td>
          {/foreach}</tr><tr>
          {foreach from=$form.basemarkers key=picname item=size}
            <td>Current Size {$size.0}x{$size.1}</td>
          {/foreach}</tr>
          </table>
        </td>
      </tr>
      <tr>
        <th><label>{g->text text="Upload a new base icon"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MC_UploadNew,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
           <input type="file" name="{g->formVar var="form[1]"}"/>
           <input type="submit" name="{g->formVar var="form[action][upload]"}" value="{g->text text="Upload a Base Marker"}" />
        </td>
      </tr>
      <tr>
        <th><label>{g->text text="New Size"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MC_SizeChange,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
            <table border=0 style='border-collapse:collapse;'>
            <tr><td><input accesskey="d" type=radio name="{g->formVar var="form[size]"}" value="same" checked onclick='DisableInputs(["ImageHeight","ImageWidth"],true);'>{g->text text="Unchange<u>d</u>"}</input></td></tr>
            <tr><td><input accesskey="5" type=radio name="{g->formVar var="form[size]"}" value="50" onclick='DisableInputs(["ImageHeight","ImageWidth"],true);'>{g->text text="<u>5</u>0%"}</input></td></tr>
            <tr><td><input accesskey="2" type=radio name="{g->formVar var="form[size]"}" value="200" onclick='DisableInputs(["ImageHeight","ImageWidth"],true);'>{g->text text="<u>2</u>00%"}</input></td></tr>
            <tr><td><input accesskey="t" type=radio name="{g->formVar var="form[size]"}" value="custom" onclick='DisableInputs(["ImageHeight","ImageWidth"],false);'>{g->text text="Cus<u>t</u>om"}</input> <input disabled type=text name="{g->formVar var="form[ImageHeight]"}" id="ImageHeight" size=3 maxlength=3/> x <input disabled type=text name="{g->formVar var="form[ImageWidth]"}" id="ImageWidth" size=3 maxlength=3/></td></tr>
            </table>
        </td>
      </tr>
      <tr>
        <th><label>{g->text text="Colors"}</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MC_ColorChange,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
            <table border=0 style='border-collapse:collapse;'>
            <tr><td><input accesskey="a" type=radio name="{g->formVar var="form[colors]"}" value=all checked onclick='DisableInputs(["blue","red","orange","green","yellow","grey","black","white","purple","aqua"],true);'>{g->text text="<u>A</u>ll Colors"}</input></td></tr>
            <tr><td><input accesskey="s" type=radio name="{g->formVar var="form[colors]"}" value=custom onclick='DisableInputs(["blue","red","orange","green","yellow","grey","black","white","purple","aqua"],false);'>{g->text text="<u>S</u>elected colors"}:</input></td></tr>
            <tr><td><input type=checkbox disabled name='blue' id='blue'/>{g->text text="Blue"}<input disabled type=checkbox name='red' id='red'/>{g->text text="Red"}<input disabled type=checkbox name='yellow' id='yellow'/>{g->text text="Yellow"}<input disabled type=checkbox name='green' id='green'/>{g->text text="Green"}<input disabled type=checkbox name='orange' id='orange'/>{g->text text="Orange"}</td></tr>
            <tr><td><input type=checkbox disabled name='aqua' id='aqua'/>{g->text text="Aqua"}<input disabled type=checkbox name='purple' id='purple'/>{g->text text="Purple"}<input disabled type=checkbox name='white' id='white'/>{g->text text="White"}<input disabled type=checkbox name='black' id='black'/>{g->text text="Black"}<input disabled type=checkbox name='grey' id='grey'/>{g->text text="Grey"}</td></tr>
            </table>
        </td>
      </tr>
      <tr>
        <th>
          <label>{g->text text="Marker Set <u>N</u>ame"}</label>
          {if isset($form.error.setname)}
              <br\><span class="giError">
                  {g->text text="Please enter the name of the set"}
              </span>
          {/if}
          {if isset($form.error.multiname)}
              <br\><span class="giError">
                  {g->text text="The name <b>multi</b> is not allowed."}
              </span>
          {/if}
        </th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_MC_MarkerName,450)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
            <table border=0 style='border-collapse:collapse;'>
            <tr><td><input accesskey="n" type=text id="SetName" name="{g->formVar var="form[setname]"}" size=25 maxlength=24/></td></tr>
            </table>
        </td>
      </tr>
  </table>
  </div>
  
  <div class="gbBlock gcBackground1">
      <input type="submit" name="{g->formVar var="form[action][generate]"}" value="{g->text text="Generate Markers"}" onClick='return verify([{$form.markerlist}]);'/>
      <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
  </div>
  {/if}
{/if}