{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock">
  <table class="gbDataTable"><tr><td>
    <input type="radio" id="rbAlbum" {if $form.linkType=='album'}checked="checked" {/if}
     name="{g->formVar var="form[linkType]"}" value="album"/>
  </td><td>
    <label for="rbAlbum">
      <b> {g->text text="Link to Album:"} </b> &nbsp;
    </label>
  </td><td>
  <div id="gTreeDiv"></div>
<script type="text/javascript">
  //<![CDATA[
  var tree;
  var nodes=[];
  var selectedId;

  function treeInit() {ldelim}
    tree = new YAHOO.widget.TreeView("gTreeDiv");
    nodes[-1] = tree.getRoot();
    selectedId = {if empty($form.linkedAlbumId)} {$ItemAddLinkItem.albumTree[0].data.id} {else} {$form.linkedAlbumId} {/if};
    {*
     * $ItemAddLinkItem contains albums in Depth-first order. Keep the ancestors of the existing
     * branch in nodes[] array in order to maintain parent ids.
     *}
    {foreach from=$ItemAddLinkItem.albumTree item=album}
      nodes[{$album.depth}] = new YAHOO.widget.TextNode({ldelim} id: "{$album.data.id}",
        label: "{$album.data.title|markup:strip|escape:javascript|default:$album.data.pathComponent}",
        href: "javascript:onLabelClick({$album.data.id})" {rdelim},
        nodes[{$album.depth-1}], {if $album.depth == 0}true{else}false{/if});
      {* If the destination album is known, expand starting with top ancestor *}
      {if $form.linkedAlbumId == $album.data.id && $album.depth > 0}
        {* NOTE: YUI requires two calls to expand a tree *}
        nodes[1].expand();
        nodes[1].expandAll();
      {/if}
    {/foreach}

    tree.draw();
    var node = tree.getNodeByProperty("id", selectedId);
    node.getLabelEl().setAttribute("class", "ygtvlabelselected");

    document.getElementById("{g->formVar var="form[linkedAlbumId]"}").value = selectedId;
  {rdelim}

  function onLabelClick(id) {ldelim}
    if (selectedId != id) {ldelim}
      var node = tree.getNodeByProperty("id", id);
      node.getLabelEl().setAttribute("class", "ygtvlabelselected");

      node = tree.getNodeByProperty("id", selectedId);
      node.getLabelEl().setAttribute("class", "ygtvlabel");

      selectedId = id;
      document.getElementById("{g->formVar var="form[linkedAlbumId]"}").value = id;
    {rdelim}
  {rdelim}

  YAHOO.util.Event.addListener(window, "load", treeInit);
  //]]>
</script>
    <input type="hidden" id="{g->formVar var="form[linkedAlbumId]"}" name="{g->formVar var="form[linkedAlbumId]"}"/>

    {if isset($form.error.linkedAlbumId.missing)}
    <div class="giError">
      {g->text text="You must enter an album id"}
    </div>
    {/if}
    {if isset($form.error.linkedAlbumId.invalid)}
    <div class="giError">
      {g->text text="Invalid album id"}
    </div>
    {/if}
  </td></tr><tr><td>
    <input type="radio" id="rbUrl" {if $form.linkType=='url'}checked="checked" {/if}
     name="{g->formVar var="form[linkType]"}" value="url"/>
  </td><td>
    <label for="rbUrl">
      <b> {g->text text="Link to External URL:"} </b>
    </label>
  </td><td>
    <input type="text" size="60"
     name="{g->formVar var="form[linkUrl]"}" value="{$form.linkUrl}"/>
    {if isset($form.error.linkUrl.missing)}
    <div class="giError">
      {g->text text="You must enter an URL"}
    </div>
    {/if}
  </td></tr></table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][addLinkItem]"}" value="{g->text text="Add Link"}"/>
</div>
