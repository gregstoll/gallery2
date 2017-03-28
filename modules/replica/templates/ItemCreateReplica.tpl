{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Create Replicas"} </h2>
</div>

{if isset($status.linked)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text one="Successfully replicated %d item" many="Successfully replicated %d items"
	   count=$status.linked.count arg1=$status.linked.count}
</h2></div>
{/if}

<div class="gbBlock">
{if empty($ItemCreateReplica.peers)}
  <p class="giDescription">
    {g->text text="This album contains no items to replicate."}
  </p>
{else}
  <p class="giDescription">
    {g->text text="A replica is an item that shares the original data file with another item, to save disk space. In all other respects it is a separate item, with its own captions, thumbnail, resizes, comments, etc. Captions are initially copied from the source item but may be changed. Either the replica or the source may be moved or deleted without affecting the other."}
  </p>

  <h3> {g->text text="Source"} </h3>

  <p class="giDescription">
    {g->text text="Choose the items you want to replicate"}
    {if ($ItemCreateReplica.numPages > 1) }
      {g->text text="(page %d of %d)"
	       arg1=$ItemCreateReplica.page
	       arg2=$ItemCreateReplica.numPages}
    {/if}
  </p>

  {if !empty($form.error.sources.empty)}
  <div class="giError">
    <h2>{g->text text="No sources chosen"}</h2>
  </div>
  {/if}

  <script type="text/javascript">
    // <![CDATA[
    function setCheck(val) {ldelim}
      var frm = document.getElementById('itemAdminForm');
      {foreach from=$ItemCreateReplica.peers item=peer}
	frm.elements['g2_form[selectedIds][{$peer.id}]'].checked = val;
      {/foreach}
    {rdelim}
    function invertCheck(val) {ldelim}
      var frm = document.getElementById('itemAdminForm');
      {foreach from=$ItemCreateReplica.peers item=peer}
	frm.elements['g2_form[selectedIds][{$peer.id}]'].checked =
	    !frm.elements['g2_form[selectedIds][{$peer.id}]'].checked;
      {/foreach}
    {rdelim}
    // ]]>
  </script>

  <table>
    <colgroup width="60"/>
    {foreach from=$ItemCreateReplica.peers item=peer}
    <tr>
      <td align="center">
	{if isset($peer.thumbnail)}
	  <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$peer.id`"}">
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
     </td>
    </tr>
    {/foreach}
  </table>

  <input type="button" class="inputTypeButton" onclick="setCheck(1)"
   name="{g->formVar var="form[action][checkall]"}" value="{g->text text="Check All"}"/>
  <input type="button" class="inputTypeButton" onclick="setCheck(0)"
   name="{g->formVar var="form[action][checknone]"}" value="{g->text text="Check None"}"/>
  <input type="button" class="inputTypeButton" onclick="invertCheck()"
   name="{g->formVar var="form[action][invert]"}" value="{g->text text="Invert"}"/>

  {if ($ItemCreateReplica.page > 1)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][previous]"}" value="{g->text text="Previous Page"}"/>
  {/if}
  {if ($ItemCreateReplica.page < $ItemCreateReplica.numPages)}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][next]"}" value="{g->text text="Next Page"}"/>
  {/if}
</div>

<div class="gbBlock">
  <h3> {g->text text="Destination"} </h3>

  <p class="giDescription">
    {g->text text="Choose a new album for the replica"}
  </p>

<div id="gTreeDiv"></div>
<script type="text/javascript">
  //<![CDATA[
  var tree;
  var nodes=[];
  var selectedId;

  function treeInit() {ldelim}
    tree = new YAHOO.widget.TreeView("gTreeDiv");
    nodes[-1] = tree.getRoot();
    selectedId = {if empty($form.destination)} {$ItemCreateReplica.albumTree[0].data.id} {else} {$form.destination} {/if};
    {*
     * $ItemCreateReplica contains albums in Depth-first order. Keep the ancestors of the existing
     * branch in nodes[] array in order to maintain parent ids.
     *}
    {foreach from=$ItemCreateReplica.albumTree item=album}
      nodes[{$album.depth}] = new YAHOO.widget.TextNode({ldelim} id: "{$album.data.id}",
        label: "{$album.data.title|markup:strip|escape:javascript|default:$album.data.pathComponent}",
        href: "javascript:onLabelClick({$album.data.id})" {rdelim},
        nodes[{$album.depth-1}], {if $album.depth == 0}true{else}false{/if});
      {* If the destination album is known, expand starting with top ancestor *}
      {if $form.destination == $album.data.id && $album.depth > 0}
        {* NOTE: YUI requires two calls to expand a tree *}
        nodes[1].expand();
        nodes[1].expandAll();
      {/if}
    {/foreach}

    tree.draw();
    var node = tree.getNodeByProperty("id", selectedId);
    node.getLabelEl().setAttribute("class", "ygtvlabelselected");

    document.getElementById("{g->formVar var="form[destination]"}").value = selectedId;
  {rdelim}

  function onLabelClick(id) {ldelim}
    if (selectedId != id) {ldelim}
      var node = tree.getNodeByProperty("id", id);
      node.getLabelEl().setAttribute("class", "ygtvlabelselected");

      node = tree.getNodeByProperty("id", selectedId);
      node.getLabelEl().setAttribute("class", "ygtvlabel");

      selectedId = id;
      document.getElementById("{g->formVar var="form[destination]"}").value = id;
    {rdelim}
  {rdelim}

  YAHOO.util.Event.addListener(window, "load", treeInit);
  //]]>
</script>
<input type="hidden" id="{g->formVar var="form[destination]"}" name="{g->formVar var="form[destination]"}"/>

  {if !empty($form.error.destination.empty)}
  <div class="giError">
    {g->text text="No destination chosen"}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="hidden" name="{g->formVar var="page"}" value="{$ItemCreateReplica.page}"/>
  <input type="hidden"
   name="{g->formVar var="form[numPerPage]"}" value="{$ItemCreateReplica.numPerPage}"/>
  {foreach from=$ItemCreateReplica.selectedIds item=selectedId}
    <input type="hidden" name="{g->formVar var="form[selectedIds][$selectedId]"}" value="on"/>
  {/foreach}

  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][link]"}" value="{g->text text="Create"}"/>
  {if $ItemCreateReplica.canCancel}
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][cancel]"}" value="{g->text text="Cancel"}"/>
  {/if}
{/if}
</div>
