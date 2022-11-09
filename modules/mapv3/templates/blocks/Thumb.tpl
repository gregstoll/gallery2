{*
 * $Revision: 1253 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<style>{literal}
    .thumbbar .vertical {
        height: 100%;
    }
    .thumbbar .vertical .tbright {
        float: right;
        margin-right: 4px;
    }
    .thumbbar .vertical .tbleft {
        float: left;
        margin-left: 4px;
    }
    .thumbbar .horizontal {
        width: 100%;
    }
    .thumbbar .horizontal .tbtop {
        margin-top: 4px;
    }
    .thumbbar .horizontal .tbbottom {
        margin-bottom: 4px;
    }
{/literal}
</style>

{assign var=vertical value=""}
{assign var=horizontal value=""}
{if $mapv3.ThumbBarPos eq "top"}
{assign var=vertical value="vertical tbtop"}
{/if}
{if $mapv3.ThumbBarPos eq "bottom"}
{assign var=vertical value="vertical tbbottom"}
{/if}
{if $mapv3.ThumbBarPos eq "right"}
{assign var=horizontal value="horizontal tbright"}
{/if}
{if $mapv3.ThumbBarPos eq "left"}
{assign var=horizontal value="horizontal tbleft"}
{/if}

<div id="thumbs" class="thumbbar {$horizontal} {$vertical}"></div>
