{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="MIME Maintenance"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"}
  {/if}
  {if isset($status.mimeSaved)}
    {g->text text="MIME entry saved successfully"}
  {/if}
  {if isset($status.deleted)}
    {g->text text="MIME entry deleted"}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    <input type="radio" id="rbAll" name="{g->formVar var="form[allowMime]"}" value="all"{if
     $form.allowMime=='all'} checked="checked"{/if} onclick="setAllowMime(this.value)"/>
    <label for="rbAll">
      {g->text text="Allow all uploads"}
    </label>
    <br/>
    <input type="radio" id="rbBlock" name="{g->formVar var="form[allowMime]"}" value="block"{if
     $form.allowMime=='block'} checked="checked"{/if} onclick="setAllowMime(this.value)"/>
    <label for="rbBlock">
      {g->text text="Block upload of types selected below"}
    </label>
    <br/>
    <input type="radio" id="rbAllow" name="{g->formVar var="form[allowMime]"}" value="allow"{if
     $form.allowMime=='allow'} checked="checked"{/if} onclick="setAllowMime(this.value)"/>
    <label for="rbAllow">
      {g->text text="Only allow uploads of types selected below"}
    </label>
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
  <input type="button" class="inputTypeButton" value="{g->text text="Add new MIME type"}"
   onclick="document.location='{g->url arg1="view=core.SiteAdmin" arg2="subView=mime.MimeEdit"}'"/>
</div>

<div class="gbBlock">
  <table id="mimeTable" class="gbDataTable" width="100%"><tr>
    <th> {g->text text="MIME Types"} </th>
    <th> {g->text text="Extensions"} </th>
    <th id="columnHeading" style="text-align: center"></th>
    <th> {g->text text="Viewable"} </th>
    <th> {g->text text="Actions"} </th>
  </tr>
  {foreach from=$MimeAdmin key=mime item=type}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {$mime}
    </td><td>
      {$type.ext}
    </td><td style="text-align: center">
      <input type="checkbox" name="{g->formVar var="form[upload][$mime]"}"{if
       $form.allowMime=='all'} checked="checked" disabled="disabled"{elseif
       isset($form.upload.$mime)} checked="checked"{/if}/>
    </td><td align="center">
      {if $type.viewable}
	<img src="{g->url href="modules/mime/data/mime_viewable.gif"}"
	 width="13" height="13" alt="{g->text text="Viewable"}" title="{g->text text="Viewable"}"/>
      {else}
	&nbsp;
      {/if}
    </td><td align="center">
      <a href="{g->url arg1="view=core.SiteAdmin" arg2="subView=mime.MimeEdit"
       arg3="mimeType=`$mime`"}">
	<img src="{g->url href="modules/mime/data/b_edit.png"}"
	 width="16" height="16" alt="{g->text text="edit"}" title="{g->text text="edit"}"/>
      </a>
      &nbsp;
      <input type="image" src="{g->url href="modules/mime/data/b_drop.png"}"
       name="{g->formVar var="form[action][delete]"}" value="{$mime}"
       alt="{g->text text="delete"}" title="{g->text text="delete"}"/>
    </td>
  </tr>
  {/foreach}
  </table>
</div>

<script type="text/javascript">
// <![CDATA[
var allowUpload = ' {g->text text="Allow Upload" forJavascript=true} ',
    blockUpload = ' {g->text text="Block Upload" forJavascript=true} ',
    allowMime = '{$form.allowMime}';
{literal}
document.getElementById('columnHeading').innerHTML =
  (allowMime == 'block') ? blockUpload : allowUpload;

function setAllowMime(key) {
  document.getElementById('columnHeading').innerHTML =
    (key == 'block') ? blockUpload : allowUpload;
  var cbs = document.getElementById('mimeTable').getElementsByTagName('INPUT');
  for (var i = 0; i < cbs.length; i++) {
    if (key == 'all') {
      cbs[i].wasChecked = cbs[i].checked;
      cbs[i].checked = cbs[i].disabled = true;
    } else {
      cbs[i].disabled = false;
      cbs[i].checked = cbs[i].wasChecked;
    }
  }
  allowMime = key;
}
{/literal}
// ]]>
</script>
