{*
 * $Revision: 16871 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if isset($ImageFrameData.data[$frame])}
  {assign var="data" value=$ImageFrameData.data[$frame]}
{/if}
{counter name="ImageFrame_counter" assign="IF_count"}
{assign var="objectId" value="IFid`$IF_count`"}
{if isset($maxSize) && isset($width) && isset($height)}
  {g->shrinkDimensions widthVar="width" heightVar="height" maxSize=$maxSize}
{/if}
{if !isset($data) || $data.type=='style'}
  {$content|replace:"%ID%":$objectId|replace:"%CLASS%":"ImageFrame_`$frame`"}
{elseif $data.type=='image'}
  {if isset($data.square) && $data.square && isset($width) && isset($height)}
    {assign var="isSquare" value=true}
    {if $width > $height}{assign var="height" value=$width}
    {else}{assign var="width" value=$height}{/if}
  {/if}
  <table class="ImageFrame_{$frame}" border="0" cellspacing="0" cellpadding="0">
  {if !empty($data.imageTT) || !empty($data.imageTL) || !empty($data.imageTR) ||
      !empty($data.imageTTL) || !empty($data.imageTTR)}
    <tr>
    <td class="TL"></td>
    {if $data.wHL}<td class="TTL"></td>{/if}
    <td class="TT"{if $data.wHL or $data.wHR
     } style="width:{if isset($width)}{$width-$data.wHL-$data.wHR}px{else}expression((document.getElementById('{$objectId}').width-{$data.wHL+$data.wHR})+'px'){/if}"
    {/if}><div class="H"></div></td>
    {if $data.wHR}<td class="TTR"></td>{/if}
    <td class="TR"></td>
    </tr>
  {/if}
  <tr>
  {capture name="LL"}
    <td class="LL"{if $data.hVT or $data.hVB
     } style="height:{if isset($height)}{$height-$data.hVT-$data.hVB}px{else}expression((document.getElementById('{$objectId}').height-{$data.hVT+$data.hVB})+'px'){/if}"
    {/if}><div class="V">&nbsp;</div></td>
  {/capture}
  {capture name="RR"}
    <td class="RR"{if $data.hVT or $data.hVB
     } style="height:{if isset($height)}{$height-$data.hVT-$data.hVB}px{else}expression((document.getElementById('{$objectId}').height-{$data.hVT+$data.hVB})+'px'){/if}"
    {/if}><div class="V">&nbsp;</div></td>
  {/capture}
  {if $data.hVT}<td class="LLT"></td>{else}{$smarty.capture.LL}{/if}
  <td rowspan="{$data.rowspan}" colspan="{$data.colspan}" class="IMG"{if isset($isSquare)
   } align="center" valign="middle" style="width:{$width}px;height:{$height}px;"
  {/if}>
  {$content|replace:"%ID%":$objectId|replace:"%CLASS%":"ImageFrame_image"}</td>
  {if $data.hVT}<td class="RRT"></td>{else}{$smarty.capture.RR}{/if}
  </tr>
  {if $data.hVT}
    <tr>
      {$smarty.capture.LL}
      {$smarty.capture.RR}
    </tr>
  {/if}
  {if $data.hVB}
    <tr>
    <td class="LLB"></td>
    <td class="RRB"></td>
    </tr>
  {/if}
  {if !empty($data.imageBB) || !empty($data.imageBL) || !empty($data.imageBR) ||
      !empty($data.imageBBL) || !empty($data.imageBBR)}
    <tr>
    <td class="BL"></td>
    {if $data.wHL}<td class="BBL"></td>{/if}
    <td class="BB"{if $data.wHL or $data.wHR
     } style="width:{if isset($width)}{$width-$data.wHL-$data.wHR}px{else}expression((document.getElementById('{$objectId}').width-{$data.wHL+$data.wHR})+'px'){/if}"
    {/if}><div class="H"></div></td>
    {if $data.wHR}<td class="BBR"></td>{/if}
    <td class="BR"></td>
    </tr>
  {/if}
  </table>
{/if}
