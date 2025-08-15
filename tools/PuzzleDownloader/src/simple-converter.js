#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

// Pure regex-based parser (no eval, no execution)
function parseTypeScriptLevelRegex(content) {
    try {
        // Extract size
        const sizeMatch = content.match(/size:\s*(\d+)/);
        if (!sizeMatch) throw new Error('No size found');
        const size = parseInt(sizeMatch[1]);
        
        // Extract colorRegions array - more robust regex
        const regionsMatch = content.match(/colorRegions:\s*\[([\s\S]*?)\],?\s*regionColors/);
        if (!regionsMatch) throw new Error('No colorRegions found');
        
        // Parse the rows
        let regionsStr = regionsMatch[1].trim();
        
        // Split by array boundaries and clean
        const rowMatches = regionsStr.match(/\[[^\]]+\]/g);
        if (!rowMatches) throw new Error('Could not parse rows');
        
        const colorRegions = rowMatches.map(rowStr => {
            // Extract quoted strings from array
            const cellMatches = rowStr.match(/"([^"]+)"/g);
            if (!cellMatches) throw new Error('Could not parse cells');
            return cellMatches.map(cell => cell.replace(/"/g, ''));
        });
        
        // Extract regionColors object
        const colorsMatch = content.match(/regionColors:\s*\{([\s\S]*?)\}/);
        if (!colorsMatch) throw new Error('No regionColors found');
        
        const regionColors = {};
        const colorsStr = colorsMatch[1];
        
        // Match key-value pairs: A: colorName,
        const colorPairs = colorsStr.match(/([A-Z]):\s*(\w+)/g);
        if (colorPairs) {
            colorPairs.forEach(pair => {
                const [, letter, color] = pair.match(/([A-Z]):\s*(\w+)/);
                regionColors[letter] = color;
            });
        }
        
        return {
            size: size,
            colorRegions: colorRegions,
            regionColors: regionColors
        };
        
    } catch (error) {
        console.error('Regex parsing failed:', error);
        return null;
    }
}

// Convert letter grid to numbers
function convertToNumbers(colorRegions, regionColors) {
    const letterToNumber = {};
    const letters = Object.keys(regionColors).sort();
    letters.forEach((letter, index) => {
        letterToNumber[letter] = index + 1;
    });
    
    return colorRegions.map(row => 
        row.map(letter => letterToNumber[letter])
    );
}

// Process local directory
function convertLocalLevels(levelsDirectory) {
    if (!fs.existsSync(levelsDirectory)) {
        console.error(`Directory not found: ${levelsDirectory}`);
        process.exit(1);
    }
    
    console.log(`Processing levels from: ${levelsDirectory}`);
    
    const files = fs.readdirSync(levelsDirectory)
        .filter(file => file.endsWith('.ts') && file.startsWith('level'))
        .sort((a, b) => {
            const levelA = parseInt(a.match(/level(\d+)/)?.[1] || '0');
            const levelB = parseInt(b.match(/level(\d+)/)?.[1] || '0');
            return levelA - levelB;
        });
    
    console.log(`Found ${files.length} level files`);
    
    let output = [];
    
    let successCount = 0;
    
    files.forEach(file => {
        try {
            console.log(`Processing ${file}...`);
            
            const filePath = path.join(levelsDirectory, file);
            const content = fs.readFileSync(filePath, 'utf8');
            const level = parseTypeScriptLevelRegex(content);
            
            if (!level || !level.colorRegions || !level.regionColors) {
                console.warn(`⚠️  Skipping ${file}: Could not parse level data`);
                return;
            }
            
            // Convert to numbered format
            const fileName = file.replace('.ts', '');
            const levelName = fileName.slice(0,-1);
            const levelNumber = fileName.slice(-1);
            // levelName.slice(0,-1);
            const numberGrid = convertToNumbers(level.colorRegions, level.regionColors);
            
            output.push(`${level.size}`);
            
            numberGrid.forEach(row => {
                output.push(row.join(" "));
            });
            
            
            // Add color mapping as comments for reference
            // output.push("# Color mapping:");
            // const letters = Object.keys(level.regionColors).sort();
            // letters.forEach((letter, idx) => {
            //     output.push(`# ${idx + 1}: ${letter} (${level.regionColors[letter]})`);
            // });
            // output.push("");
            
            successCount++;
            
        } catch (error) {
            console.error(`❌ Error processing ${file}:`, error.message);
        }
    });
    
    // Write output
    const outputFile = '../puzzles.txt';
    fs.writeFileSync(outputFile, output.join('\n'));
    
    console.log(`\n✅ Success! Converted ${successCount}/${files.length} levels`);
    console.log(`📁 Output saved to: ${outputFile}`);
    console.log(`📊 File size: ${fs.statSync(outputFile).size} bytes`);
}

// Main execution
if (require.main === module) {
    const args = process.argv.slice(2);
    
    if (args.length === 0) {
        console.error('Usage: node simple-converter.js <path-to-levels-directory>');
        console.error('Example: node simple-converter.js ./queens-game-linkedin/src/utils/levels');
        process.exit(1);
    }
    
    const levelsDir = args[0];
    convertLocalLevels(levelsDir);
}

module.exports = { parseTypeScriptLevelRegex, convertToNumbers, convertLocalLevels };
