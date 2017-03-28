{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Watermark Images"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Watermark updated successfully"}
  {/if}
  {if isset($status.delete)}
    {g->text text="Image deleted successfully"}
  {/if}
  {if isset($status.addError)}
    <span class="giError">{g->text text="Missing image file"}</span>
  {/if}
  {if isset($status.savedSettings)}
    {g->text text="Settings saved successfully"}
  {/if}
  {if isset($status.missingDefault)}
    <br/><span class="giWarning">{g->text text="Make sure to select a default watermark"}</span>
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <table class="gbDataTable"><tr>
    <td>
      <input type="checkbox" {if $form.allowUserWatermarks}checked="checked" {/if}
       name="{g->formVar var="form[allowUserWatermarks]"}" id="allowUserWatermarks"
       {if $form.forceDefaultWatermark}disabled="disabled"{/if}/>
    </td><td>
      <label for="allowUserWatermarks">
	{g->text text="Allow users to upload their own watermark images"}
      </label>
    </td></tr><tr><td>
      <input type="checkbox" {if $form.forceDefaultWatermark}checked="checked" {/if}
       name="{g->formVar var="form[forceDefaultWatermark]"}" id="forceDefaultWatermark"
       onclick="document.getElementById('allowUserWatermarks').disabled=this.checked"/>
    </td><td>
      <label for="forceDefaultWatermark">
	{g->text text="Use only the default watermark selected below.  Only Site Administrators may change or remove watermarks."}
      </label>
    </td>
  </tr></table>

  <table class="gbDataTable" width="100%"><tr>
    <th> {g->text text="File"} </th>
    <th> {g->text text="Image"} </th>
    <th> {g->text text="Owner"} </th>
    <th> {g->text text="Action"} </th>
    <th> {g->text text="Hotlink"} </th>
    <th> {g->text text="Default"} </th>
  </tr>
  {foreach from=$form.list item=item}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {$item.name}
    </td><td>
      {g->image item=$item image=$item maxSize=150}
    </td><td>
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=core.AdminEditUser"
		arg3="userId=`$item.ownerId`"}">
	{$WatermarkSiteAdmin.owners[$item.ownerId].fullName|default:$WatermarkSiteAdmin.owners[$item.ownerId].userName}
      </a>
    </td><td>
      <a href="{g->url arg1="view=core.SiteAdmin"
		arg2="subView=watermark.WatermarkSiteAdminEdit" arg3="watermarkId=`$item.id`"}">
	{g->text text="edit"}
      </a>
      &nbsp;
      <a href="{g->url arg1="controller=watermark.WatermarkSiteAdmin"
		arg2="form[action][delete]=1" arg3="form[delete][watermarkId]=`$item.id`"}">
	{g->text text="delete"}
      </a>
    </td><td style="text-align:center">
      <input type="radio" {if $item.id == $form.hotlinkWatermarkId}checked="checked" {/if}
       name="{g->formVar var="form[hotlinkWatermarkId]"}" value="{$item.id}"/>
    </td><td style="text-align:center">
      <input type="radio" {if $item.id == $form.defaultWatermarkId}checked="checked" {/if}
       name="{g->formVar var="form[defaultWatermarkId]"}" value="{$item.id}"/>
    </td>
  </tr>
  {/foreach}

  <tr><td colspan="4">
    <input type="file" size="60" name="{g->formVar var="form[1]"}"/>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][add]"}" value="{g->text text="Add"}"/>
  </td><td style="text-align:center">
    {if !empty($form.list)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
    {/if}
  </td><td style="text-align:center">
    {if !empty($form.list)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
    {/if}
  </td></tr>
  </table>

  {if !empty($form.list)}
  <strong> {g->text text="Watermark Hotlinked Images"} </strong>
  <p class="giDescription">
    {g->text text="Gallery can automatically apply a watermark to images linked from sites outside your Gallery. Select a watermark above and then activate hotlink watermarks using the Rewrite module."}
  </p>
  {/if}
</div>
