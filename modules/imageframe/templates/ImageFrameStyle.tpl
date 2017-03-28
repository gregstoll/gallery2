{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
img.ImageFrame_image {ldelim} vertical-align:bottom; border:none; {rdelim}
{foreach from=$ImageFrameData.data key=id item=data}
{if $data.type=='style'}
img.ImageFrame_{$id} {ldelim} {$data.style} {rdelim}
{elseif $data.type=='image'}
table.ImageFrame_{$id} {ldelim} direction: ltr; {rdelim}
{if !empty($data.imageTL)}table.ImageFrame_{$id} .TL {ldelim} width:{$data.widthTL}px; height:{$data.heightTL}px; background:url({$data.imageTL}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageTTL)}table.ImageFrame_{$id} .TTL {ldelim} width:{$data.widthTTL}px; background:url({$data.imageTTL}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageTT)}table.ImageFrame_{$id} .TT {ldelim} height:{$data.heightTT}px; background:url({$data.imageTT}) repeat-x; {rdelim}
{/if}
{if !empty($data.imageTTR)}table.ImageFrame_{$id} .TTR {ldelim} width:{$data.widthTTR}px; background:url({$data.imageTTR}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageTR)}table.ImageFrame_{$id} .TR {ldelim} width:{$data.widthTR}px; height:{$data.heightTR}px; background:url({$data.imageTR}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageLLT)}table.ImageFrame_{$id} .LLT {ldelim} height:{$data.heightLLT}px; background:url({$data.imageLLT}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageLL)}table.ImageFrame_{$id} .LL {ldelim} width:{$data.widthLL}px; background:url({$data.imageLL}) repeat-y; {rdelim}
table.ImageFrame_{$id} .LL div.V {ldelim} width:{$data.widthLL}px; {rdelim}
{/if}
{if !empty($data.imageLLB)}table.ImageFrame_{$id} .LLB {ldelim} height:{$data.heightLLB}px; background:url({$data.imageLLB}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageRRT)}table.ImageFrame_{$id} .RRT {ldelim} height:{$data.heightRRT}px; background:url({$data.imageRRT}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageRR)}table.ImageFrame_{$id} .RR {ldelim} width:{$data.widthRR}px; background:url({$data.imageRR}) repeat-y; {rdelim}
table.ImageFrame_{$id} .RR div.V {ldelim} width:{$data.widthRR}px; {rdelim}
{/if}
{if !empty($data.imageRRB)}table.ImageFrame_{$id} .RRB {ldelim} height:{$data.heightRRB}px; background:url({$data.imageRRB}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageBL)}table.ImageFrame_{$id} .BL {ldelim} width:{$data.widthBL}px; height:{$data.heightBL}px; background:url({$data.imageBL}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageBBL)}table.ImageFrame_{$id} .BBL {ldelim} width:{$data.widthBBL}px; background:url({$data.imageBBL}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageBB)}table.ImageFrame_{$id} .BB {ldelim} height:{$data.heightBB}px; background:url({$data.imageBB}) repeat-x; {rdelim}
{/if}
{if !empty($data.imageBBR)}table.ImageFrame_{$id} .BBR {ldelim} width:{$data.widthBBR}px; background:url({$data.imageBBR}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageBR)}table.ImageFrame_{$id} .BR {ldelim} width:{$data.widthBR}px; height:{$data.heightBR}px; background:url({$data.imageBR}) no-repeat; {rdelim}
{/if}
{if !empty($data.imageCC)}table.ImageFrame_{$id} .IMG {ldelim} background:url({$data.imageCC}) repeat center center; {rdelim}
{/if}
table.ImageFrame_{$id} td {ldelim} font-size:1px {rdelim} /* For IE */
{/if}
{/foreach}
td div.H {ldelim} width:1px; height:0; {rdelim}
td div.V {ldelim} width:0; height:1px; {rdelim}
