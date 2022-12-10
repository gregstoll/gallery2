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
{if !empty($form.error)}
<div class="gbBlock">
   <h2 class="giError">{g->text text="There was a problem processing your request."}</h2>
</div>
{/if}


<!--
End of error/success displaying
-->

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="Group"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->

{if $form.mode eq 'AddGroup'}

<div class="gbBlock">
  <h3> {g->text text="Group Settings"}</h3>
  <p class="giDescription">
    {g->text text="Settings specific to this Google Map Group"}
  </p>
  {if isset($form.OldGroup)}
  <input type="hidden" name="edit" value="{$form.OldGroup}"/>
  <input type="hidden" name="oldid" value="{$form.EditGroup.0}"/>
  {/if}
   <table class="gbDataTable">
   <tr>
     <th><label for="GroupTitle">{g->text text="Enter Group Title"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_Name,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
        <input type="text" name="GroupTitle" {if isset($form.OldGroup) and isset($form.EditGroup.1)}value="{$form.EditGroup.1}"{/if}/> *
        {if isset($form.error.grouptitle)}
          <span class="giError">
          {g->text text="Please enter a group title."}
          </span>
        {/if}
        </td>
        <td width=50px rowspan=5>&nbsp;</td>
        <th>{g->text text="Group Thumbnail"}</td>
   </tr>
   <tr>
     <th><label for="GroupDate">{g->text text="Enter a creation date for the group"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_Date,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input type="text" name="GroupDate" value="{if isset($form.OldGroup)}{$form.EditGroup.5}{else}{$form.now}{/if}"/> <i>{g->text text="Default is today's date"}</i>
        </td>
        <td rowspan=4>
        {if isset($form.OldGroup)}
           <img width=140px src={$form.EditGroup.4}/><input type="hidden" name="pic" value={$form.EditGroup.4}/>
        {else}
           <center>None<center>
        {/if}
        </td>
   </tr>
   <tr>
     <th><label for="GroupSumm">{g->text text="Enter a Summary for this group"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_Summary,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input name="GroupSumm" type="text" size="50" {if isset($form.OldGroup) and isset($form.EditGroup.3)}value="{$form.EditGroup.3}"{/if}/>
        </td>
   </tr>
   <tr>
     <th><label for="GroupPic">{g->text text="Upload a Thumbnail"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_Thumb,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
         <input type="file" name="{g->formVar var="form[3]"}"/>
         <input type="submit" name="{g->formVar var="form[action][uploadpic]"}" value="{g->text text="Upload and Set"}" />
        </td>
   </tr>
   <tr>
     <th><label for="GroupDesc">{g->text text="Enter a Group Description"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_Description,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <textarea name="GroupDesc" cols=38 rows=4>{if isset($form.OldGroup) and isset($form.EditGroup.2)}{$form.EditGroup.2}{/if}</textarea>
        </td>
   </tr>
   <tr>
     <th><label for="GroupGPS">{g->text text="GPS coordinates for the marker"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_GPS,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td colspan=3>
          <input type="text" name="GroupGPS" size=40 {if isset($form.OldGroup) and isset($form.EditGroupParam.GPS)}value="{$form.EditGroupParam.GPS}"{/if}/>  <input type="submit" name="{g->formVar var="form[action][getcoord]"}" value="{g->text text="Get via a Map"}"/>
        </td>
   </tr>
   <tr>
     <th><label for="GroupZoomlevel">{g->text text="ZoomLevel for 'Zoom-in' link"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_ZoomIn,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input type="text" name="GroupZoomlevel" {if isset($form.OldGroup) and isset($form.EditGroupParam.ZoomLevel)}value="{$form.EditGroupParam.ZoomLevel}"{/if}/>
        </td>
   </tr>
   <tr>
     <th><label for="GroupColor">{g->text text="Marker Color"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_G_Color,350)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <select name="GroupColor">
            {$form.colorGoption}
          </select>
        </td>
   </tr>
  </table>
</div>

<div class="gbBlock">
  <h3>
{if isset($form.AdminHelp) and $form.AdminHelp eq 1}
      <img onclick="javascript:showhelp(_HP_G_Items,600)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/>
  {/if}
  {g->text text="Items in Group"}
  </h3>
  {if isset($form.items)}
  <table class="gbDataTable itemtable">
  <tr><th colspan=2>{g->text text="Select the items you want to be part of this group"}</th></tr>
  {foreach from=$form.items item=item key=num}
  <tr onmouseover="Showpic('{$item.id}')" onmouseout="Hidepic('{$item.id}')" bgcolor="{if $item.type eq 'GoogleMapGroup'}lightblue{/if}{if $item.type eq 'GalleryPhotoItem'}pink{/if}{if $item.type eq 'GalleryAlbumItem'}lightgreen{/if}">
  <td><font color=black>{$item.title}</font></td>
  <td><input name="P:{$item.id}" type="checkbox" value="{$item.id}"
     {if isset($form.OldGroup)}
       {foreach from=$form.EditGroup item=grouppoint}
         {if $grouppoint eq $item.id}checked{/if}
       {/foreach}
     {/if}
  /></td>
  </tr>
  <div style="position:absolute;right:50px;display:none" id="P{$item.id}"><img src="{$item.thumb}"/></div>
  {/foreach}
  </table>
  {else}
   {g->text text="There are no items with GPS coordinates. Please add coordinates to at least one item to create a group."}
  {/if}
</div>

<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Group"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>

{else}

<div class="gbBlock">
  <h3> {g->text text="Group Management"} </h3>

  <p class="giDescription">
    <img {if !$form.IE}' src="{$form.picbase}information.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}information.png");' src='{$form.picbase}blank.gif'{/if}/>
    {g->text text="This section allows you to manage Groups. A Group is shown as a marker on the main Map. When clicking its thumbnail, it will not open a Photo Page or an Album page but another Map with the items selected to be in that group."}
  </p>
   {if isset($form.thegroups) and $form.thegroups != ""}
     <table class="gbDataTable">
       <tr><th>{g->text text="Group ID"}</th><th>{g->text text="Group Name"}</th><th>{g->text text="Group Creation Date"}</th><th>{g->text text="Group Thumbnail"}</th><th colspan=2>{g->text text="Actions"}</th></tr>
       {foreach from=$form.thegroups item=group}
          <tr><td>{$group[0]}</td><td>{$group[1]}</td><td>{$group[5]}</td><td>{$group[4]}</td>
          <td><input type="submit" name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete Group %s" arg1=$group[0]}" /></td>
          <td><input type="submit" name="{g->formVar var="form[action][edit]"}" value="{g->text text="Edit Group %s" arg1=$group[0]}" /></td>
          </tr>
       {/foreach}
     </table>
   {else}
     {g->text text="There are no Groups created, click"} <input type="submit" name="{g->formVar var="form[action][add]"}" value="{g->text text="Add" hint="Create new"}" />
   {/if}
</div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][add]"}" value="{g->text text="Add Group"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>
{/if}