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
{if isset($status.filtersaved)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Filter successfully saved"}</h2>
    </div>
{/if}
{if isset($status.filterdeleted)}
    <div class="gbBlock">
        <h2 class="giSuccess">{g->text text="Filter successfully deleted"}</h2>
    </div>
{/if}

<!--
End of error/success displaying
-->

{include file="modules/mapv3/templates/MapAdminTab.tpl" mode="Filter"}

<!-- Help Div -->
<div id="helpdiv" style="visibility:hidden;border:2px solid black;position:absolute;right:350px;top:200px;width:250px;height:150px;">
    <div id="helphead" style="width:250px;height:25px;background:#150095;color:white;border-bottom:2px solid black;"><img alt="none" {if !$form.IE}style="float:left;" src="{$form.picbase}help.png"{else}src='{$form.picbase}blank.gif' style='float:left;filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");'{/if}/><table style="height:100%;border-collapse:collapse;"><tr><td style="width:230px;vertical-align:middle;"><b>Help Window</b></td><td style="width:25px;cursor:pointer;" onclick="javascript:hidehelp()"><center><b>X</b></center></td></tr></table></div>
    <div id="helptext" style="width:250px;height:123px;color:black;font-size:12px;background:#B2E4F6;-moz-opacity:.75;filter:alpha(opacity=75);opacity:.75;overflow:auto;clear:both;">
    </div>
</div>
<!-- End of Help Div -->


{if $form.mode eq 'AddFilter'}
<div class="gbBlock">
  <h3> {g->text text="Filters Management"} </h3>
  {if isset($form.OldFilter)}
  <input type="hidden" name="edit" value="{$form.OldFilter}"/>
  <input type="hidden" name="oldid" value="{$form.EditFilter.0}"/>
  {/if}
  <p class="giDescription">
    {g->text text="This section allows you to manage Filters"}
  </p>
   <table class="gbDataTable">
   <tr>
     <th><label for="FilterName">{g->text text="Filter <u>N</u>ame"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_AddName,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
     <td>
        <input accesskey="n" type="text" name="FilterName" {if isset($form.OldFilter)}value="{$form.EditFilter.1}"{/if}/> *
        {if isset($form.error.filtername)}
        <span class="giError">
        {g->text text="Please enter a filter name"}
        </span>
        {/if}
     </td>
   </tr>
   <tr>
     <th><label for="FilterZoom">{g->text text="Base <u>Z</u>oom"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_AddZoom,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
     <td>
        <input accesskey="z" size="3" type="text" name="FilterZoom"   {if isset($form.OldFilter)}value="{$form.EditFilter.2}"{/if}/>
        {if isset($form.error.filterzoom)}
        <span class="giError">
        {g->text text="Please enter a zoom value between 0 and 19"}
        </span>
        {/if}
     </td>
   </tr>
   <tr>
     <th><label for="FilterCenter">{g->text text="Base Center"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_AddGPS,200)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
     <td>
        <input size="40" type="text" name="FilterCenter"   {if isset($form.OldFilter)}value="{$form.EditFilter.3}"{/if}/> *
          <input type="hidden" name="center" value="{if isset($form.OldFilter)}{$form.EditFilter.3}{/if}"/>
          <input type="hidden" name="zoom" value="{if isset($form.OldFilter)}{$form.EditFilter.2}{/if}"/>
          <input type="submit" name="{g->formVar var="form[action][getviamap]"}" value="{g->text text="Get via a Map"}"/>
        {if isset($form.error.filtercenter)}
        <span class="giError">
        {g->text text="Bad coordinates, please verify"}
        </span>
        {/if}
     </td>
   </tr>
  </table>
</div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}" value="{g->text text="Save Filter"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>

{else}

<div class="gbBlock">
  <h3> {g->text text="Filters Management"} </h3>

  <p class="giDescription">
    {g->text text="This section allows you to manage Filters"}
  </p>
   <table class="gbDataTable">
   <tr>
     <th><label for="ShowFilters">{g->text text="Filter Block Position"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_BlockPos,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <select name="ShowFilters">
          <option value="0">{g->text text="Hide" hint="Don't display"}</option>
          <option value="top" {if $form.ShowFilters eq "top"}selected{/if}>{g->text text="Top" hint="Opposite of bottom"}</option>
          <option value="right" {if $form.ShowFilters eq "right"}selected{/if}>{g->text text="Right" hint="Opposite of left"}</option>
          <option value="bottom" {if $form.ShowFilters eq "bottom"}selected{/if}>{g->text text="Bottom" hint="Opposite of top"}</option>
          <option value="left" {if $form.ShowFilters eq "left"}selected{/if}>{g->text text="Left" hint="Opposite of right"}</option>
        </td>
   </tr>
   <tr>
     <th><label for="FilterList">{g->text text="Manual Filter List"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_ManualFilter,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          {if isset($form.filterlist) and !empty($form.filterlist)}
            <select name="{g->formVar var="form[filter]"}">
            {foreach item=option from=$form.filterlist}
              <option label={$option.name} value="{$option.id}">{$option.name}</option>
            {/foreach}
            </select>
            <input type="submit" name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
            <input type="submit" name="{g->formVar var="form[action][edit]"}" value="{g->text text="Edit"}"/>
          {else}
            <span class="giError">{g->text text="There are no filters defined, click add to create one"}
            </span>
          {/if}
            <input type="submit" name="{g->formVar var="form[action][add]"}" value="{g->text text="Add" hint="Create new"}"/>
        </td>
   </tr>
   <tr>
     <th><label for="ShowAlbumFilters">{g->text text="Create <u>A</u>lbum Filters"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_AlbumFilter,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
        <td>
          <input accesskey="a" type="checkbox" name ="showalbumfilters" value="1"{if $form.ShowAlbumFilters}checked{/if}/>
        </td>
   </tr>
    <tr>
      <th><label for="LevelFilterAll">{g->text text="Set Album Filter Depth"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_AlbumFilterDepth,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
         <td>
           <input type="Text" name="{g->formVar var="form[LevelFilterAll]"}" value="{$form.LevelFilterAll}"/>
         </td>
    </tr>
    <tr>
      <th><label for="LevelFilterRoot">{g->text text="Root Album shows only 1 level"}:</label></th>
            {if isset($form.AdminHelp) and $form.AdminHelp eq 1}
            <th><img onclick="javascript:showhelp(_HP_FI_AlbumFilterDepthRoot,250)" alt="help" style='cursor:pointer;{if !$form.IE}' src="{$form.picbase}help.png" {else}filter: progid:DXImageTransform.Microsoft.AlphaImageLoader(src="{$form.picbase}help.png");' src='{$form.picbase}blank.gif'{/if}/></th>
            {/if}            
         <td>
           <input type="checkbox" name="{g->formVar var="form[LevelFilterRoot]"}" value="1" {if $form.LevelFilterRoot}checked{/if}/>
         </td>
    </tr>
 </table>
</div>
<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][savesettings]"}" value="{g->text text="Save Settings"}"/>
    <input type="submit" name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel" hint="Discard changes"}"/>
</div>
{/if}