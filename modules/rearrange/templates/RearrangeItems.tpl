{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{*
 * Precalculate the HTML that will go inside every cell in the
 * table so that we can render it twice (once in the Javascript,
 * once in the HTML itself.  We do this to avoid using the innerHTML
 * element when we swap the cells because the behavior of innerHTML
 * varies from platform to platform and on Firefox it turns ~ into %7E
 * which breaks our cookie paths.
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Rearrange Album"} </h2>
</div>

{if isset($RearrangeItems.automaticOrderMessage)}
<div class="gbBlock">
  <p class="giDescription">
    {g->text text="This album has an automatic sort order specified, so you cannot change the order of items manually.  You must remove the automatic sort order to continue."}
    <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemEdit"
     arg3="editPlugin=ItemEditAlbum" arg4="itemId=`$ItemAdmin.item.id`"}">
      {g->text text="change"}
    </a>
  </p>
</div>
{else}

<script type="text/javascript">
// <![CDATA[
var sel = -1, list = new Array();
var html = new Array();
{foreach from=$RearrangeItems.children key=idx item=child}
  {capture name="html"}
  {include file="gallery:modules/rearrange/templates/RearrangeItemsCell.tpl" child=$child}
  {/capture}
  html[{$idx}] = '{$smarty.capture.html|escape:javascript}';
{/foreach}
for (var i = 0; i < {$RearrangeItems.count}; i++) {literal} {
  list[i] = i;
}
function save() {
  var s = '';
  for (var i = 0; i < list.length; i++) {
    if (i > 0) s += ',';
    s += list[i];
  }
  document.getElementById('riList').value = s;
}
function doclick(idx) {
  if (sel < 0) {
    sel = idx;
    document.getElementById('item_'+sel).getElementsByTagName('*')[0].style.borderColor = '#ff3333';
    document.getElementById('item_'+sel).parentNode.style.backgroundColor = '#ff3333';
  } else {
    var a = document.getElementById('item_'+sel);
    a.getElementsByTagName('*')[0].style.borderColor = 'black';
    a.parentNode.style.backgroundColor = 'transparent';
    if (idx != sel) {
      var dir = (sel < idx) ? 1 : -1, tt, ti, i, b;
      ti = list[sel];
      tt = html[sel];
      for (i = sel; i != idx; a = b, i += dir) {
        b = document.getElementById('item_' + (i+dir));
        a.innerHTML = html[i] = html[i+dir];
        list[i] = list[i+dir];
      }
      a.innerHTML = html[i] = tt;
      list[idx] = ti;
    }
    sel = -1;
  }
}
// ]]>
{/literal}</script>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Order saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Change the order of the items in this album.%s Click an item to move, then click the new location." arg1="<br/>"}
  </p>

  {if $RearrangeItems.columns > 0}
    <table class="rearrangeTable">
  {else}
    <div class="rearrangeTable">
  {/if}

  {assign var="row" value=0}
  {assign var="column" value=0}
  {foreach name=childList from=$RearrangeItems.children key=idx item=child}
    {if $RearrangeItems.columns > 0}
      {if $column == 0} <tr> {/if}
      <td>
    {else}
      <div class="riFloat"
	   style="width:{$RearrangeItems.maxWidth}px;height:{$RearrangeItems.maxHeight}px">
    {/if}

    <a href="#" id="item_{$idx}" onclick="doclick({$idx});return false">
      {include file="gallery:modules/rearrange/templates/RearrangeItemsCell.tpl"
	       child=$child l10Domain="modules_rearrange"}
    </a>

    {if $RearrangeItems.columns > 0}
      </td>
      {assign var="column" value=$column+1}
      {if $column == $RearrangeItems.columns || $smarty.foreach.childList.last}
	</tr>
	{assign var="column" value=0}
	{assign var="row" value=$row+1}
	{if $row == $RearrangeItems.rows}
	  <tr><td colspan="{$RearrangeItems.columns}"><hr/></td></tr>
	  {assign var="row" value=0}
	{/if}
      {/if}
    {else}
      </div>
    {/if}
  {/foreach}

  {if $RearrangeItems.columns > 0}
    </table>
  {else}
    </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="hidden" id="riList" name="{g->formVar var="form[list]"}" value=""/>
  <input type="submit" class="inputTypeSubmit" onclick="save()"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
{/if}
