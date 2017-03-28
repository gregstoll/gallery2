{*
 * $Revision: 17739 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="albumselect.LoadAlbumData" albumTree=true stripTitles=true}

{if isset($block.albumselect)}
<div class="{$class}">
  <div class="dtree">
    {assign var="params" value=$block.albumselect.LoadAlbumData.albumTree.params}
    {assign var="albumTree" value=$block.albumselect.LoadAlbumData.albumTree.albumTreeName}
    {if $params.treeExpandCollapse and !$params.treeCloseSameLevel}
      <p>
	<a href="javascript: {$albumTree}.openAll()"
	 onclick="this.blur()">{g->text text="Expand"}</a>
	|
	<a href="javascript: {$albumTree}.closeAll()"
	 onclick="this.blur()">{g->text text="Collapse"}</a>
      </p>
    {/if}

    <script type="text/javascript">
      // <![CDATA[
      function albumSelect_goToNode(nodeId) {ldelim}
        document.location = new String('{g->url arg1="view=core.ShowItem" arg2="itemId=__ID__" htmlEntities=false}').replace('__ID__', nodeId);
      {rdelim}

      var {$albumTree} = new dTree('{$albumTree}');
      var {$albumTree}_images = '{g->url href="modules/albumselect/images/"}'
      {$albumTree}.icon = {ldelim}
	  root            : {$albumTree}_images + 'base.gif',
	  folder          : {$albumTree}_images + 'folder.gif',
	  folderOpen      : {$albumTree}_images + 'imgfolder.gif',
	  node            : {$albumTree}_images + 'imgfolder.gif',
	  empty           : {$albumTree}_images + 'empty.gif',
	  line            : {$albumTree}_images + 'line{$params.rtl}.gif',
	  join            : {$albumTree}_images + 'join{$params.rtl}.gif',
	  joinBottom      : {$albumTree}_images + 'joinbottom{$params.rtl}.gif',
	  plus            : {$albumTree}_images + 'plus{$params.rtl}.gif',
	  plusBottom      : {$albumTree}_images + 'plusbottom{$params.rtl}.gif',
	  minus           : {$albumTree}_images + 'minus{$params.rtl}.gif',
	  minusBottom     : {$albumTree}_images + 'minusbottom{$params.rtl}.gif',
	  nlPlus          : {$albumTree}_images + 'nolines_plus.gif',
	  nlMinus         : {$albumTree}_images + 'nolines_minus.gif'
      {rdelim};
      {$albumTree}.config.useLines = {if $params.treeLines}true{else}false{/if};
      {$albumTree}.config.useIcons = {if $params.treeIcons}true{else}false{/if};
      {$albumTree}.config.useCookies = {if $params.treeCookies}true{else}false{/if};
      {$albumTree}.config.closeSameLevel = {if $params.treeCloseSameLevel}true{else}false{/if};
      {$albumTree}.config.cookiePath = '{$block.albumselect.cookiePath}';
      {$albumTree}.config.cookieDomain = '{$block.albumselect.cookieDomain}';
      {assign var="data" value=$block.albumselect.LoadAlbumData.albumTree}
      {$albumTree}.add(0, -1, " {$data.titlesForJs.root}", '{g->url}');
      {ldelim} var pf = '{$data.links.prefix}';
      {foreach from=$data.tree item=node}
	{$albumTree}.add({$node.nodeId}, {$node.parentNode}, "{$data.titlesForJs[$node.id]}", pf+'{$data.links[$node.id]}');
      {/foreach} {rdelim}
      document.write({$albumTree});
      // ]]>
    </script>
  </div>
</div>
{/if}
