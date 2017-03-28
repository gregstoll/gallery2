{*
 * $Revision: 17172 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if $callCount == 1}
<script type="text/javascript" src="{g->url href="lib/yui/utilities.js"}"></script>
<script type="text/javascript" src="{g->url href="lib/yui/autocomplete-min.js"}"></script>
<script type="text/javascript" src="{g->url href="lib/javascript/AutoComplete.js"}"></script>
{/if}
<script type="text/javascript">
  // <![CDATA[
  YAHOO.util.Event.addListener(
    this, 'load',
    function(e, data) {ldelim} autoCompleteAttach(data[0], data[1]); {rdelim},
    ['{$element}', '{$url}']);
  // ]]>
</script>

