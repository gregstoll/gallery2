{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Delete Items"} </h2>
</div>

{if (isset($status.deleted))}
<div class="gbBlock">
  {if ($status.deleted.count == 0)}
    <h2 class="giError">
      {g->text text="No items were selected for deletion"}
  {else}
    <h2 class="giSuccess">
      {g->text one="Successfully deleted %d item" many="Successfully deleted %d items"
	       count=$status.deleted.count arg1=$status.deleted.count}
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
{if empty($ItemDelete.peers)}
  <p class="giDescription">
    {g->text text="This album contains no items to delete"}
  </p>
{else}
  <p class="giDescription">
    {g->text text="Choose the items you want to delete"}
    {if ($ItemDelete.numPages > 1) }
      {g->text text="(page %d of %d)" arg1=$ItemDelete.page arg2=$ItemDelete.numPages}
      <br/>
      {g->text text="Items selected here will remain selected when moving between pages."}
      {if !empty($ItemDelete.selectedIds)}
	 <br/>
	 {g->text one="One item selected on other pages." many="%d items selected on other pages."
		  count=$ItemDelete.selectedIdCount arg1=$ItemDelete.selectedIdCount}
      {/if}
    {/if}
  </p>

  <input type="hidden" name="{g->formVar var="page"}" value="{$ItemDelete.page}"/>
  <input type="hidden" name="{g->formVar var="form[formname]"}" value="DeleteItem"/>

  <script type="text/javascript">
    // <![CDATA[
    function setCheck(val) {ldelim}
      var frm = document.getElementById('itemAdminForm');
      {foreach from=$ItemDelete.peers item=peer}
	frm.elements['g2_form[selectedIds][{$peer.id}]'].checked = val;
      {/foreach}
    {rdelim}
    function invertCheck(val) {ldelim}
      var frm = document.getElementById('itemAdminForm');
      {foreach from=$ItemDelete.peers item=peer}
	frm.elements['g2_form[selectedIds][{$peer.id}]'].checked =
	    !frm.elements['g2_form[selectedIds][{$peer.id}]'].checked;
      {/foreach}
    {rdelim}
    // ]]>
  </script>

  <table>
    <colgroup width="60"/>
    {foreach from=$ItemDelete.peers item=peer}
    {cycle values="1,2" assign="alternate"}
    {if $alternate==1}<tr><td style="text-align: center">{else}<td style="padding-left:50px; text-align: center">{/if}
      
	{if isset($peer.thumbnail)}
	  <a id="thumb_{$peer.id}" href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$peer.id`"}">
	    {g->image item=$peer image=$peer.thumbnail maxSize=50 class="giThumbnail"}
	  </a>
	{else}
	  &nbsp;
	{/if}
      </td><td>
	<input type="checkbox" id="cb_{$peer.id}" {if $peer.selected}checked="checked" {/if}
	 name="{g->formVar var="form[selectedIds][`$peer.id`]"}"/>
      </td><td>
	<label for="cb_{$peer.id}">
	  {$peer.title|markup:strip|default:$peer.pathComponent}
	</label>
	<i>
	  {if isset($ItemDelete.peerTypes.data[$peer.id])}
	    {g->text text="(data)"}
	  {/if}
	  {if isset($ItemDelete.peerTypes.album[$peer.id])}
	    {if isset($ItemDelete.peerDescendentCounts[$peer.id])}
	      {g->text one="(album containing %d item)" many="(album containing %d items)"
		       count=$ItemDelete.peerDescendentCounts[$peer.id]
		       arg1=$ItemDelete.peerDescendentCounts[$peer.id]}
	    {else}
	      {g->text text="(empty album)"}
	    {/if}
	  {/if}
	</i>
      </td>
    {if $alternate==2}</tr>{/if}
    {/foreach}
    {if $alternate==1}<td colspan="3">&nbsp;</td></tr>{/if}
  </table>
    <script type="text/javascript">
      //<![CDATA[
      {foreach from=$ItemDelete.peers item=peer}
      {if isset($peer.resize)}
      {* force and alt/longdesc parameter here so that we avoid issues with single quotes in the title/description *}
      new YAHOO.widget.Tooltip("gTooltip", {ldelim}
          context: "thumb_{$peer.id}", text: '{g->image item=$peer image=$peer.resize class="giThumbnail" maxSize=500 alt="" longdesc="" }',
          showDelay: 250 {rdelim});
      {elseif isset($peer.thumbnail)}
      new YAHOO.widget.Tooltip("gTooltip", {ldelim}
          context: "thumb_{$peer.id}", text: '{g->image item=$peer image=$peer.thumbnail class="giThumbnail" alt="" longdesc=""}',
          showDelay: 250 {rdelim});
      {/if}
      {/foreach}
      //]]>
  </script>

  {foreach from=$ItemDelete.selectedIds item=selectedId}
    <input type="hidden" name="{g->formVar var="form[selectedIds][$selectedId]"}" value="on"/>
  {/foreach}
  <input type="hidden" name="{g->formVar var="form[numPerPage]"}" value="{$ItemDelete.numPerPage}"/>

  <br/>

  <input type="button" class="inputTypeButton" onclick="setCheck(1)"
   name="{g->formVar var="form[action][checkall]"}" value="{g->text text="Check All"}"/>
  <input type="button" class="inputTypeButton" onclick="setCheck(0)"
   name="{g->formVar var="form[action][checknone]"}" value="{g->text text="Check None"}"/>
  <input type="button" class="inputTypeButton" onclick="invertCheck()"
   name="{g->formVar var="form[action][invert]"}" value="{g->text text="Invert"}"/>

  {if ($ItemDelete.page > 1)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][previous]"}" value="{g->text text="Previous Page"}"/>
  {/if}
  {if ($ItemDelete.page < $ItemDelete.numPages)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][next]"}" value="{g->text text="Next Page"}"/>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][delete]"}" value="{g->text text="Delete"}"/>
  {if $ItemDelete.canCancel}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {/if}
{/if}
</div>
