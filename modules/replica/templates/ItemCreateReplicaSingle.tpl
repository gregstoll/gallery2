{*
 * $Revision: 17070 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Replicate %s" arg1=$ItemCreateReplicaSingle.itemTypeNames.0} </h2>
</div>

{if isset($status.linked)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Successfully created"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="A replica is an item that shares the original data file with another item, to save disk space. In all other respects it is a separate item, with its own captions, thumbnail, resizes, comments, etc. Captions are initially copied from the source item but may be changed. Either the replica or the source may be moved or deleted without affecting the other."}
  </p>

  <h3> {g->text text="Destination"} </h3>

  <p class="giDescription">
    {g->text text="Choose a destination album"}
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
    selectedId = {if empty($form.destination)} {$ItemCreateReplicaSingle.albumTree[0].data.id} {else} {$form.destination} {/if};
    {*
     * $ItemCreateReplicaSingle contains albums in Depth-first order. Keep the ancestors of the existing
     * branch in nodes[] array in order to maintain parent ids.
     *}
    {foreach from=$ItemCreateReplicaSingle.albumTree item=album}
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

  {if isset($form.error.destination.empty)}
  <div class="giError">
    {g->text text="No destination chosen"}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][link]"}" value="{g->text text="Create"}"/>
</div>
